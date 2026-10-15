add_subdirectory(common)
add_subdirectory(cpp_api)
add_subdirectory(lua_api)

# Used by server and client
set(common_SCRIPT_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/scripting_server.cpp
	${common_SCRIPT_COMMON_SRCS}
	${common_SCRIPT_CPP_API_SRCS}
	${common_SCRIPT_LUA_API_SRCS}
	PARENT_SCOPE)

