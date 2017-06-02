if(NOT TARGET nonius_dl)
	include(ExternalProject)
	ExternalProject_Add(nonius_dl
		PREFIX ${CMAKE_BINARY_DIR}/nonius
		GIT_REPOSITORY https://github.com/rmartinho/nonius.git
		GIT_TAG stable
		TIMEOUT 10
		UPDATE_COMMAND ${GIT_EXECUTABLE} pull origin v1.1.2
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
		LOG_DOWNLOAD ON
		)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/nonius/src/nonius_dl/include)

find_package(Boost REQUIRED)

add_library(nonius_boost INTERFACE IMPORTED)
set_target_properties(nonius_boost PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
)

add_library(nonius INTERFACE IMPORTED)
set_target_properties(nonius PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/nonius/src/nonius_dl/include
	INTERFACE_LINK_LIBRARIES nonius_boost
)
add_dependencies(nonius nonius_dl)
