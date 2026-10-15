cmake_minimum_required(VERSION 3.12)

# This can be read from ${PROJECT_NAME} after project() is called
project(squareone)
set(PROJECT_NAME_CAPITALIZED "SquareOne Core")
set(PROJECT_PACKAGE_NAME "squareone-core")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(GCC_MINIMUM_VERSION "5.1")
set(CLANG_MINIMUM_VERSION "3.5")

# Also remember to set PROTOCOL_VERSION in network/networkprotocol.h when releasing
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 6)
set(VERSION_EXTRA "" CACHE STRING "Stuff to append to version string")

# Change to false for releases
set(DEVELOPMENT_BUILD FALSE)

set(ENABLE_UPDATE_CHECKER FALSE CACHE BOOL
	"Whether to enable update checks by default")

set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
if(VERSION_EXTRA)
	set(VERSION_STRING ${VERSION_STRING}-${VERSION_EXTRA})
elseif(DEVELOPMENT_BUILD)
	set(VERSION_STRING "${VERSION_STRING}-dev")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	# Append "-debug" to version string
	set(VERSION_STRING "${VERSION_STRING}-debug")
endif()

message(STATUS "*** Will build version ${VERSION_STRING} ***")


# Configuration options
set(DEFAULT_RUN_IN_PLACE FALSE)
if(WIN32)
	set(DEFAULT_RUN_IN_PLACE TRUE)
endif()
set(RUN_IN_PLACE ${DEFAULT_RUN_IN_PLACE} CACHE BOOL
	"Run directly in source directory structure")


if(BUILD_CLIENT)
	message(FATAL_ERROR "SquareOne Core is server-only; client builds are no longer supported")
endif()
set(BUILD_CLIENT FALSE)
set(BUILD_UNITTESTS FALSE CACHE BOOL "Build unittests")


set(WARN_ALL TRUE CACHE BOOL "Enable -Wall for Release build")

if(NOT CMAKE_BUILD_TYPE)
	# Default to release
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

# Included stuff
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


# This is done here so that relative search paths are more reasonable
find_package(Irrlicht REQUIRED)


# Installation

if(WIN32)
	set(SHAREDIR ".")
	set(BINDIR "bin")
	set(DOCDIR "doc")
	set(EXAMPLE_CONF_DIR ".")
	set(LOCALEDIR "locale")
elseif(UNIX) # Linux, BSD etc
	if(RUN_IN_PLACE)
		set(SHAREDIR ".")
		set(BINDIR "bin")
		set(DOCDIR "doc")
		set(EXAMPLE_CONF_DIR ".")
		set(MANDIR "unix/man")
		set(LOCALEDIR "locale")
	else()
		set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")
		set(BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
		set(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}")
		set(MANDIR "${CMAKE_INSTALL_PREFIX}/share/man")
		set(EXAMPLE_CONF_DIR ${DOCDIR})
		set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")
	endif()
endif()

set(CUSTOM_SHAREDIR "" CACHE STRING "Directory to install data files into")
if(NOT CUSTOM_SHAREDIR STREQUAL "")
	set(SHAREDIR "${CUSTOM_SHAREDIR}")
	message(STATUS "Using SHAREDIR=${SHAREDIR}")
endif()

set(CUSTOM_BINDIR "" CACHE STRING "Directory to install binaries into")
if(NOT CUSTOM_BINDIR STREQUAL "")
	set(BINDIR "${CUSTOM_BINDIR}")
	message(STATUS "Using BINDIR=${BINDIR}")
endif()

set(CUSTOM_DOCDIR "" CACHE STRING "Directory to install documentation into")
if(NOT CUSTOM_DOCDIR STREQUAL "")
	set(DOCDIR "${CUSTOM_DOCDIR}")
	if(NOT RUN_IN_PLACE)
		set(EXAMPLE_CONF_DIR ${DOCDIR})
	endif()
	message(STATUS "Using DOCDIR=${DOCDIR}")
endif()

set(CUSTOM_MANDIR "" CACHE STRING "Directory to install manpages into")
if(NOT CUSTOM_MANDIR STREQUAL "")
	set(MANDIR "${CUSTOM_MANDIR}")
	message(STATUS "Using MANDIR=${MANDIR}")
endif()

set(CUSTOM_EXAMPLE_CONF_DIR "" CACHE STRING "Directory to install example config file into")
if(NOT CUSTOM_EXAMPLE_CONF_DIR STREQUAL "")
	set(EXAMPLE_CONF_DIR "${CUSTOM_EXAMPLE_CONF_DIR}")
	message(STATUS "Using EXAMPLE_CONF_DIR=${EXAMPLE_CONF_DIR}")
endif()

set(CUSTOM_LOCALEDIR "" CACHE STRING "Directory to install l10n files into")
if(NOT CUSTOM_LOCALEDIR STREQUAL "")
	set(LOCALEDIR "${CUSTOM_LOCALEDIR}")
	message(STATUS "Using LOCALEDIR=${LOCALEDIR}")
endif()


install(DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/async"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/game"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/profiler"
	DESTINATION "${SHAREDIR}/builtin")
install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/after.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/chatcommands.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/information_formspecs.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/misc_helpers.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/serialize.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/strict.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/common/vector.lua"
	DESTINATION "${SHAREDIR}/builtin/common")
install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/init.lua"
	"${CMAKE_CURRENT_SOURCE_DIR}/builtin/settingtypes.txt"
	DESTINATION "${SHAREDIR}/builtin")
if(RUN_IN_PLACE)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/mods/mods_here.txt" DESTINATION "${SHAREDIR}/mods")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/minetest_game" DESTINATION "${SHAREDIR}/games/"
	COMPONENT "SUBGAME_MINETEST_GAME" OPTIONAL PATTERN ".git*" EXCLUDE )
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/devtest" DESTINATION "${SHAREDIR}/games/"
	COMPONENT "SUBGAME_MINIMAL" OPTIONAL PATTERN ".git*" EXCLUDE )

install(FILES "README.md" DESTINATION "${DOCDIR}" COMPONENT "Docs")
install(FILES "doc/lua_api.txt" DESTINATION "${DOCDIR}" COMPONENT "Docs")
install(FILES "doc/world_format.txt" DESTINATION "${DOCDIR}" COMPONENT "Docs")
install(FILES "squareone.conf.example" DESTINATION "${EXAMPLE_CONF_DIR}")

if(UNIX)
	install(FILES "doc/squareoneserver.6" DESTINATION "${MANDIR}/man6")
endif()

# Library pack
find_package(GMP REQUIRED)
find_package(Json REQUIRED)
find_package(Lua REQUIRED)
add_subdirectory(lib/chacha)
add_subdirectory(lib/luautf8)
add_subdirectory(lib/luachacha)

# JsonCPP doesn't compile well on GCC 4.8
if(NOT ENABLE_SYSTEM_JSONCPP)
	set(GCC_MINIMUM_VERSION "4.9")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "${GCC_MINIMUM_VERSION}")
		message(FATAL_ERROR "Insufficient gcc version, found ${CMAKE_CXX_COMPILER_VERSION}. "
			"Version ${GCC_MINIMUM_VERSION} or higher is required.")
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "${CLANG_MINIMUM_VERSION}")
		message(FATAL_ERROR "Insufficient clang version, found ${CMAKE_CXX_COMPILER_VERSION}. "
			"Version ${CLANG_MINIMUM_VERSION} or higher is required.")
	endif()
endif()

# Subdirectories
# Be sure to add all relevant definitions above this

add_subdirectory(src)


# CPack

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A free open-source voxel game engine with easy modding and game creation.")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_NAME "${PROJECT_PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "SquareOne Core contributors")

include(CPackComponent)

cpack_add_component(Docs
	DISPLAY_NAME "Documentation"
	DESCRIPTION "Documentation about SquareOne Core and Minetest-compatible modding"
)

cpack_add_component(SUBGAME_MINETEST_GAME
	DISPLAY_NAME "Minetest Game"
	DESCRIPTION "The default game bundled in the Minetest engine. Mainly used as a modding base."
	GROUP "Games"
)

cpack_add_component(SUBGAME_MINIMAL
	DISPLAY_NAME "Development Test"
	DESCRIPTION "A basic testing environment used for engine development and sometimes for testing mods."
	DISABLED #DISABLED does not mean it is disabled, and is just not selected by default.
	GROUP "Games"
)

cpack_add_component_group(Subgames
	DESCRIPTION "Games for SquareOne Core."
)

if(WIN32)
	# Include all dynamically linked runtime libaries such as MSVCRxxx.dll
	include(InstallRequiredSystemLibraries)

	if(RUN_IN_PLACE)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(CPACK_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}-${VERSION_STRING}-win64")
		else()
			set(CPACK_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}-${VERSION_STRING}-win32")
		endif()

		set(CPACK_GENERATOR ZIP)

	else()
		set(CPACK_GENERATOR WIX)
		set(CPACK_PACKAGE_NAME "${PROJECT_NAME_CAPITALIZED}")
		set(CPACK_PACKAGE_INSTALL_DIRECTORY ".")
		set(CPACK_PACKAGING_INSTALL_PREFIX "/${PROJECT_NAME_CAPITALIZED}")

		set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/misc/squareone-icon.ico")
		# Supported languages can be found at
		# http://wixtoolset.org/documentation/manual/v3/wixui/wixui_localization.html
		#set(CPACK_WIX_CULTURES "ar-SA,bg-BG,ca-ES,hr-HR,cs-CZ,da-DK,nl-NL,en-US,et-EE,fi-FI,fr-FR,de-DE")
		set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/misc/CPACK_WIX_UI_BANNER.BMP")
		set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/misc/CPACK_WIX_UI_DIALOG.BMP")

		set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/doc/lgpl-2.1.txt")

		# The correct way would be to include both x32 and x64 into one installer
		# and install the appropriate one.
		# CMake does not support that, so there are two separate GUID's
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(CPACK_WIX_UPGRADE_GUID "AC81E261-B797-49D3-8BDB-B24F9AC6BE62")
		else()
			set(CPACK_WIX_UPGRADE_GUID "439FFC1A-A6D0-48EC-B897-D2F1A29661BF")
		endif()
	endif()
elseif(APPLE)
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}-${VERSION_STRING}-osx")
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}-${VERSION_STRING}-linux")
	set(CPACK_GENERATOR TGZ)
	set(CPACK_SOURCE_GENERATOR TGZ)
endif()

include(CPack)


# Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
			${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif()
