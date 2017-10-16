if(JDUTIL_COVERAGEHELPERS OR TARGET coverage_reset)
	return()
endif()
set(JDUTIL_COVERAGEHELPERS 1)

set(coverage_dir ${CMAKE_BINARY_DIR}/coverage)

### Detect Browser

if(APPLE OR MSVC)
	set(JDUTIL_WEB_BROWSER "open")
else()
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		if(LINUX_DISTRO MATCHES "Debian")
			set(default_web_browser "sensible-browser")
		else()
			set(default_web_browser "xdg-open")
		endif()
	else()
		set(default_web_browser "firefox")
	endif()
	set(JDUTIL_WEB_BROWSER ${default_web_browser} CACHE STRING "Command to open web browser")
endif()

### Coverage

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(base_opts "--directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh")

	find_program(LLVM_COV_EXECUTABLE llvm-cov)
	find_program(LCOV_EXECUTABLE lcov)
	find_program(GENHTML_EXECUTABLE genhtml)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/data/llvm-gcov-wrapper.sh.in ${coverage_dir}/llvm-gcov-wrapper.sh)

	function(add_coverage_flags)
		foreach(target ${ARGN})
			target_compile_options(${target} PUBLIC --coverage)
			set_target_properties(${target} PROPERTIES LINK_FLAGS "--coverage")
		endforeach()
	endfunction()

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

	function(add_coverage_capture_target target)
		set(coverage_dir ${CMAKE_BINARY_DIR}/coverage)
		add_custom_target(coverage_capture_${target}
			DEPENDS coverage_base
			COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage_reset
			COMMAND ${target}
			COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh --capture --base-directory ${CMAKE_BINARY_DIR} --test-name ${target} -o ${coverage_dir}/${target}.info
			COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --gcov-tool ${coverage_dir}/llvm-gcov-wrapper.sh -a ${coverage_dir}/base.info -a ${coverage_dir}/${target}.info -o ${coverage_dir}/combined_test_${target}.info
			VERBATIM
			COMMENT "Generating coverage for ${target}"
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
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	function(add_coverage_flags)
		foreach(target ${ARGN})
			target_compile_options(${target} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
			set_target_properties(${target} PROPERTIES LINK_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
		endforeach()
	endfunction()
	function(add_coverage_capture_target target)
		add_coverage_flags(${target})

		set(profraw ${coverage_dir}/${target}.profraw)
		set(profdata ${coverage_dir}/${target}.profdata)
		add_custom_command(OUTPUT ${profdata}
			COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${profraw} $<TARGET_FILE:${target}>
			COMMAND xcrun llvm-profdata merge ${profraw} -o ${profdata}
			VERBATIM
			BYPRODUCTS ${profraw}
			DEPENDS ${target}
		)
		add_custom_command(TARGET ${target} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E remove ${profdata} ${profraw}
		)
		add_custom_target(coverage_capture_${target}
			DEPENDS ${profdata} ${target}
		)
	endfunction()
	function(add_coverage_capture name)
		set(output "${coverage_dir}/${name}.profdata")
		set(opts)
		set(depends)
		set(binaries)
		foreach(target ${ARGN})
			set(opts "${opts} -weighted-input=1,${coverage_dir}/${target}.profdata")
			set(depends "${depends} ${coverage_dir}/${target}.profdata coverage_capture_${target}")
			list(APPEND binaries $<TARGET_FILE:${target}>)
		endforeach()
		separate_arguments(opts UNIX_COMMAND "${opts}")
		separate_arguments(depends UNIX_COMMAND "${depends}")
		add_custom_command(OUTPUT ${output}
			COMMAND xcrun llvm-profdata merge -output=${output} ${opts}
			VERBATIM
			DEPENDS ${depends}
		)
		add_custom_target(coverage_${name}_html
			DEPENDS ${output}
			COMMAND xcrun llvm-cov show ${binaries} -instr-profile=${coverage_dir}/${name}.profdata -format=html -output-dir=${coverage_dir}/html
			VERBATIM
			COMMENT "Generating HTML for ${name} coverage..."
		)
		add_custom_target(coverage_${name}_open
			DEPENDS coverage_${name}_html
			COMMAND ${JDUTIL_WEB_BROWSER} ${coverage_dir}/html/index.html
			VERBATIM
		)
	endfunction()
else()
	function(add_coverage_capture_target)
	endfunction()
	function(add_coverage_capture)
	endfunction()
	function(add_coverage_flags)
endif()
