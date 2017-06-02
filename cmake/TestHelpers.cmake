if(JDUTIL_TESTHELPERS)
	return()
endif()
set(JDUTIL_TESTHELPERS 1)

include(CoverageHelpers)

function(add_unit_test name)
	if(NOT DEFINED JDUTIL_TEST_DIR)
		set(JDUTIL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	add_executable(tst_${name} ${JDUTIL_TEST_DIR}/tst_${name}.cpp)
	set_source_files_properties(tst_${name}.cpp PROPERTIES COMPILE_FLAGS "-Wno-missing-prototypes")
	target_link_libraries(tst_${name} catch jd-util ${JDUTIL_TEST_LIBS})
	add_test(NAME tst_${name} COMMAND tst_${name})
	add_coverage_capture_target(${name} tst_${name})
endfunction()

function(add_benchmark name)
	if(NOT DEFINED JDUTIL_TEST_DIR)
		set(JDUTIL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	add_executable(ben_${name} ${JDUTIL_TEST_DIR}/ben_${name}.cpp)
	target_link_libraries(ben_${name} nonius pthread jd-util ${JDUTIL_TEST_LIBS})
	add_test(NAME ben_${name} COMMAND ben_${name})
endfunction()
