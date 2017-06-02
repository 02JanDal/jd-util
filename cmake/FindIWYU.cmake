find_program(IWYU_EXECUTABLE NAMES include-what-you-use iwyu)

if(IWYU_EXECUTABLE)
	if(NOT EXISTS ${CMAKE_BINARY_DIR}/iwyu-qt5.imp)
		file(DOWNLOAD https://rawgit.com/include-what-you-use/include-what-you-use/master/qt5_4.imp ${CMAKE_BINARY_DIR}/iwyu-qt5.imp)
	endif()

	set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXECUTABLE} -Xiwyu --mapping_file=${CMAKE_BINARY_DIR}/iwyu-qt5.imp)
	execute_process(
		COMMAND ${IWYU_EXECUTABLE} --version VERBATIM
		OUTPUT_VARIABLE version_raw
	)
	string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" IWYU_VERSION_STRING "${version_raw}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IWYU REQUIRED_VARS IWYU_EXECUTABLE VERSION_VAR IWYU_VERSION_STRING)
