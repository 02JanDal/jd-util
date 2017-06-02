if(NOT TARGET catch_dl)
	include(ExternalProject)
	ExternalProject_Add(catch_dl
		PREFIX ${CMAKE_BINARY_DIR}/catch
		GIT_REPOSITORY https://github.com/philsquared/Catch.git
		TIMEOUT 10
		UPDATE_COMMAND ${GIT_EXECUTABLE} pull
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
		LOG_DOWNLOAD ON
		)
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/catch/src/catch_dl/single_include)
add_library(catch INTERFACE IMPORTED)
set_target_properties(catch PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/catch/src/catch_dl/single_include)
add_dependencies(catch catch_dl)
