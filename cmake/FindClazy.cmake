if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	find_library(JDUTIL_CLAZY_PLUGIN_LIB ClangLazy.so ClangLazy.dylib)

	if(NOT JDUTIL_CLAZY_PLUGIN_LIB AND JDUTIL_BUILD_CLAZY)
		include(ExternalProject)
		ExternalProject_Add(clazy
			PREFIX ${CMAKE_BINARY_DIR}/clazy
			GIT_REPOSITORY https://github.com/KDE/clazy.git
			TIMEOUT 10
			UPDATE_COMMAND ${GIT_EXECUTABLE} pull origin 1.1
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/clazy/install
			LOG_DOWNLOAD ON
			)

		if(CMAKE_HOST_APPLE)
			set(_host_suffix "dylib")
		elseif(CMAKE_HOST_UNIX)
			set(_host_suffix "so")
		elseif(CMAKE_HOST_WIN32)
			set(_host_suffix "dll")
		endif()

		set(JDUTIL_CLAZY_PLUGIN_LIB "${CMAKE_BINARY_DIR}/clazy/install/lib/ClangLazy.${_host_suffix}")
	endif()

	set(JDUTIL_CLAZY_ARGUMENTS "-Xclang -load -Xclang ${JDUTIL_CLAZY_PLUGIN_LIB} -Xclang -add-plugin -Xclang clang-lazy")

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Clazy DEFAULT_MSG JDUTIL_CLAZY_PLUGIN_LIB)
else()
	message(WARNING "Clazy is only supported on clang")
endif()
