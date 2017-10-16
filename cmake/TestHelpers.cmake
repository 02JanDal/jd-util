if(JDUTIL_TESTHELPERS)
	return()
endif()
set(JDUTIL_TESTHELPERS 1)

include(CoverageHelpers)
include(CMakeParseArguments)

function(add_unit_test name)
	set(options)
	set(oneValueArgs DIR)
	set(multiValueArgs LIBRARIES EXTRASRC)
	cmake_parse_arguments(AUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if("${AUT_DIR}" STREQUAL "")
		if(NOT DEFINED JDUTIL_TEST_DIR)
			set(AUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
		else()
			set(AUT_DIR ${JDUTIL_TEST_DIR})
		endif()
	endif()

	add_executable(tst_${name} ${AUT_DIR}/tst_${name}.cpp ${AUT_EXTRASRC})
	set_source_files_properties(tst_${name}.cpp PROPERTIES COMPILE_FLAGS "-Wno-missing-prototypes")
	target_link_libraries(tst_${name} PRIVATE catch jd-util ${JDUTIL_TEST_LIBS} ${AUT_LIBRARIES})
	target_compile_definitions(tst_${name} PRIVATE CATCH_CONFIG_MAIN)
	add_test(NAME tst_${name} COMMAND tst_${name})
	add_coverage_capture_target(tst_${name})
endfunction()

function(add_benchmark name)
	if(NOT DEFINED JDUTIL_TEST_DIR)
		set(JDUTIL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	add_executable(ben_${name} ${JDUTIL_TEST_DIR}/ben_${name}.cpp)
	target_link_libraries(ben_${name} nonius pthread jd-util ${JDUTIL_TEST_LIBS})
	add_test(NAME ben_${name} COMMAND ben_${name})
endfunction()
