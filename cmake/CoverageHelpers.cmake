if(JDUTIL_COVERAGEHELPERS OR TARGET coverage_reset)
	return()
endif()
set(JDUTIL_COVERAGEHELPERS 1)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(coverage_dir ${CMAKE_BINARY_DIR}/coverage)
	set(base_opts "--directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh")

	find_program(LLVM_COV_EXECUTABLE llvm-cov)
	find_program(LCOV_EXECUTABLE lcov)
	find_program(GENHTML_EXECUTABLE genhtml)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/data/llvm-gcov-wrapper.sh.in ${coverage_dir}/llvm-gcov-wrapper.sh)

	add_custom_target(coverage_reset
		COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh --zerocounters
		VERBATIM
		COMMENT "Reseting coverage counters"
	)
	add_custom_target(coverage_base
		DEPENDS coverage_reset
		COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh --capture --base-directory ${CMAKE_BINARY_DIR} --initial -o ${coverage_dir}/base.info
		VERBATIM
		COMMENT "Generating base profile for coverage"
	)

	function(add_coverage_capture_target test command)
		set(coverage_dir ${CMAKE_BINARY_DIR}/coverage)
		add_custom_target(coverage_capture_${test}
			DEPENDS coverage_base
			COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage_reset
			COMMAND ${command}
			COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh --capture --base-directory ${CMAKE_BINARY_DIR} --test-name ${test} -o ${coverage_dir}/${test}.info
			COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh -a ${coverage_dir}/base.info -a ${coverage_dir}/${test}.info -o ${coverage_dir}/combined_test_${test}.info
			VERBATIM
			COMMENT "Generating coverage for ${test}"
		)
	endfunction()

	function(add_coverage_capture name)
		set(coverage_dir ${CMAKE_BINARY_DIR}/coverage)
		set(base_opts "--directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh")
		foreach(test ${ARGN})
			list(APPEND dependencies "coverage_capture_${test}")
			set(opts "${opts} -a ${coverage_dir}/combined_test_${test}.info")
		endforeach()
		separate_arguments(base_opts UNIX_COMMAND "${base_opts}")
		separate_arguments(opts UNIX_COMMAND "${opts}")
		add_custom_command(
			OUTPUT ${coverage_dir}/cleaned_${name}.info
			DEPENDS coverage_reset ${dependencies}
			COMMAND ${LCOV_EXECUTABLE} ${base_opts} ${opts} -o ${coverage_dir}/combined_${name}.info
			COMMAND ${LCOV_EXECUTABLE} ${base_opts} --remove ${coverage_dir}/combined_${name}.info "/usr/*" "*/build/*" "*/test/*" -o ${coverage_dir}/cleaned_${name}.info
			VERBATIM
			COMMENT "Combining coverage files for ${name}"
		)
		get_filename_component(prefix "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
		add_custom_target(coverage_html_${name}
			DEPENDS ${coverage_dir}/cleaned_${name}.info
			COMMAND ${GENHTML_EXECUTABLE} ${coverage_dir}/cleaned_${name}.info -o ${coverage_dir}/${name} --prefix ${prefix}
			VERBATIM
			COMMENT "Generating HTML coverage report for ${name}"
		)
	endfunction()

	mark_as_advanced(LLVM_COV_EXECUTABLE LCOV_EXECUTABLE GENHTML_EXECUTABLE)
else()
	function(add_coverage_capture_target)
	endfunction()
	function(add_coverage_capture)
	endfunction()
endif()
