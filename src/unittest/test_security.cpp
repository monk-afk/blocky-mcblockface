/*
Minetest
Copyright (C) 2026 SquareOne Core contributors

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

#include "script/cpp_api/s_security.h"

class TestSecurityScript : public ScriptApiSecurity
{
public:
	explicit TestSecurityScript(ScriptingType type) : ScriptApiBase(type) {}

	lua_State *getLuaState() { return getStack(); }
};

class TestSecurity : public TestBase
{
public:
	TestSecurity() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestSecurity"; }

	void runTests(IGameDef *gamedef);

	void testSecureFunctionEnvironments();
	void testClientGetfenvRemoved();
};

static TestSecurity g_test_instance;

void TestSecurity::runTests(IGameDef *gamedef)
{
	TEST(testSecureFunctionEnvironments);
	TEST(testClientGetfenvRemoved);
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

////////////////////////////////////////////////////////////////////////////////

void TestSecurity::testSecureFunctionEnvironments()
{
	TestSecurityScript script(ScriptingType::Server);
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

void TestSecurity::testClientGetfenvRemoved()
{
	TestSecurityScript script(ScriptingType::Client);
	script.initializeSecurityClient();

	std::string error;
	UTEST(runLua(script.getLuaState(),
			      "assert(getfenv == nil)\n"
			      "assert(type(setfenv) == 'function')\n"
			      "assert(os.execute == nil)\n",
			      &error),
			"Client sandbox probe failed: %s", error.c_str());
}
