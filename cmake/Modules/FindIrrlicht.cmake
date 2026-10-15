
mark_as_advanced(IRRLICHT_INCLUDE_DIR)
set(IRRLICHT_SOURCE_DIR "" CACHE PATH "Path to irrlicht source directory (optional)")


# Find include directory

if(NOT IRRLICHT_SOURCE_DIR STREQUAL "")
	set(IRRLICHT_SOURCE_DIR_INCLUDE
		"${IRRLICHT_SOURCE_DIR}/include"
	)

	find_path(IRRLICHT_INCLUDE_DIR NAMES irrlicht.h
		PATHS
		${IRRLICHT_SOURCE_DIR_INCLUDE}
		NO_DEFAULT_PATH
	)

else()
	find_path(IRRLICHT_INCLUDE_DIR NAMES irrlicht.h
		PATHS
		/usr/local/include/irrlicht
		/usr/include/irrlicht
		/system/develop/headers/irrlicht #Haiku
		PATH_SUFFIXES "include/irrlicht"
	)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Irrlicht DEFAULT_MSG IRRLICHT_INCLUDE_DIR)

