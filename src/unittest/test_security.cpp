/*
Minetest
Copyright (C) 2026 Blocky McBlockface contributors

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3.0 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "test.h"

#include "config.h"
#include "filesys.h"
#include "script/cpp_api/s_security.h"
#include "script/lua_api/l_http.h"
#include "script/lua_api/l_util.h"
#include "settings.h"

class TestSecurityScript : public ScriptApiSecurity
{
public:
	TestSecurityScript() : ScriptApiBase() {}

	lua_State *getLuaState() { return getStack(); }

	void initializeModApis()
	{
		lua_State *L = getStack();
		lua_getglobal(L, "core");
		int top = lua_gettop(L);
		ModApiUtil::Initialize(L, top);
		ModApiHttp::Initialize(L, top);
		lua_pop(L, 1);
	}
};

class ScopedSetting
{
public:
	ScopedSetting(const std::string &name, const std::string &value) :
			m_name(name), m_existed(g_settings->getNoEx(name, m_previous))
	{
		g_settings->set(name, value);
	}

	~ScopedSetting()
	{
		if (m_existed)
			g_settings->set(m_name, m_previous);
		else
			g_settings->remove(m_name);
	}

private:
	std::string m_name;
	std::string m_previous;
	bool m_existed;
};

class TestSecurity : public TestBase
{
public:
	TestSecurity() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestSecurity"; }

	void runTests(IGameDef *gamedef);

	void testSecureFunctionEnvironments();
	void testInsecureEnvironmentRejectsCoroutine();
#if USE_CURL
	void testHttpApiAuthorization();
#endif
};

static TestSecurity g_test_instance;

void TestSecurity::runTests(IGameDef *gamedef)
{
	TEST(testSecureFunctionEnvironments);
	TEST(testInsecureEnvironmentRejectsCoroutine);
#if USE_CURL
	TEST(testHttpApiAuthorization);
#endif
}

static bool runLua(lua_State *L, const char *code, std::string *error)
{
	int result = luaL_loadstring(L, code);
	if (result == 0)
		result = lua_pcall(L, 0, 0, 0);
	if (result == 0)
		return true;

	const char *message = lua_tostring(L, -1);
	*error = message ? message : "unknown Lua error";
	lua_pop(L, 1);
	return false;
}

static void loadTestMod(TestSecurityScript &script, const std::string &path,
		const std::string &name, const std::string &code)
{
	UASSERT(fs::safeWriteToFile(path, code));
	script.loadMod(path, name);
}

////////////////////////////////////////////////////////////////////////////////

void TestSecurity::testSecureFunctionEnvironments()
{
	TestSecurityScript script;
	script.initializeSecurity();
	lua_State *L = script.getLuaState();

	// The retained insecure environment must be distinct and remain usable by
	// trusted mods, but must not expose the secure core table.
	lua_getglobal(L, "_G");
	lua_rawgeti(L, LUA_REGISTRYINDEX, CUSTOM_RIDX_GLOBALS_BACKUP);
	UASSERT(!lua_rawequal(L, -1, -2));
	lua_getfield(L, -1, "core");
	UASSERT(lua_isnil(L, -1));
	lua_pop(L, 1);
	lua_getfield(L, -1, "os");
	lua_getfield(L, -1, "execute");
	UASSERT(lua_isfunction(L, -1));
	lua_pop(L, 4);

	const char *probe = "assert(type(getfenv) == 'function')\n"
			    "assert(type(setfenv) == 'function')\n"
			    "assert(getfenv(getfenv) == _G)\n"
			    "assert(getfenv(string.byte) == _G)\n"
			    "assert(getfenv(table.insert) == _G)\n"
			    "assert(getfenv(math.abs) == _G)\n"
			    "assert(getmetatable('').__index == string)\n"
			    "assert(getfenv(getmetatable('').__index.byte) == _G)\n"
			    "assert(os.execute == nil)\n"
			    "assert(io.popen == nil)\n";
	std::string error;
	UTEST(runLua(L, probe, &error), "Sandbox probe failed: %s", error.c_str());
}

void TestSecurity::testInsecureEnvironmentRejectsCoroutine()
{
	ScopedSetting trusted_mods("secure.trusted_mods", "trusted");
	TestSecurityScript script;
	script.initializeSecurity();
	script.initializeModApis();

	const std::string path = getTestTempFile();
	loadTestMod(script, path, "trusted",
			"assert(type(core.request_insecure_environment()) == 'table')\n");

	loadTestMod(script, path, "attacker",
			"local original = core.request_insecure_environment\n"
			"core.request_insecure_environment = function()\n"
			"  local thread = coroutine.create(function() return original() end)\n"
			"  local ok, result = coroutine.resume(thread)\n"
			"  assert(ok)\n"
			"  stolen_insecure_environment = result\n"
			"  return result\n"
			"end\n");

	loadTestMod(script, path, "trusted",
			"assert(core.request_insecure_environment() == nil)\n"
			"assert(stolen_insecure_environment == nil)\n");
}

#if USE_CURL
void TestSecurity::testHttpApiAuthorization()
{
	ScopedSetting trusted_mods("secure.trusted_mods", "trusted");
	TestSecurityScript script;
	script.initializeSecurity();
	script.initializeModApis();
	lua_State *L = script.getLuaState();

	std::string error;
	UTEST(runLua(L,
			"core.set_http_api_lua(function(httpenv)\n"
			"  httpenv.registered_helper = true\n"
			"  return httpenv\n"
			"end)\n"
			"core.set_http_api_lua = nil\n",
			&error), "HTTP API setup failed: %s", error.c_str());

	const std::string path = getTestTempFile();
	loadTestMod(script, path, "trusted",
			"local http = core.request_http_api()\n"
			"assert(type(http) == 'table')\n"
			"assert(http.registered_helper == true)\n");

	loadTestMod(script, path, "attacker",
			"core.http_add_fetch = function(httpenv)\n"
			"  stolen_http_helper = httpenv\n"
			"  return httpenv\n"
			"end\n"
			"original_http_request = core.request_http_api\n");

	loadTestMod(script, path, "trusted",
			"local http = core.request_http_api()\n"
			"assert(type(http) == 'table')\n"
			"assert(http.registered_helper == true)\n"
			"assert(stolen_http_helper == nil)\n");

	loadTestMod(script, path, "attacker",
			"core.request_http_api = function()\n"
			"  local thread = coroutine.create(function()\n"
			"    return original_http_request()\n"
			"  end)\n"
			"  local ok, result = coroutine.resume(thread)\n"
			"  assert(ok)\n"
			"  stolen_http_api = result\n"
			"  return result\n"
			"end\n");

	loadTestMod(script, path, "trusted",
			"assert(core.request_http_api() == nil)\n"
			"assert(stolen_http_api == nil)\n");
}
#endif
