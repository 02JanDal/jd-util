if(JDUTIL_CPPCHECK)
	return()
endif()
set(JDUTIL_CPPCHECK 1)

find_program(CPPCHECK_EXECUTABLE cppcheck)
mark_as_advanced(CPPCHECK_EXECUTABLE)

if(CPPCHECK_EXECUTABLE)
	function(cppcheck_target target)
		if(TARGET ${target})
			gather_flags_for_target(${target})

			get_target_property(srcs ${target} SOURCES)
			foreach(src ${srcs})
				if(NOT src MATCHES ".*\.ui$")
					set(opts "${opts} ${CMAKE_CURRENT_SOURCE_DIR}/${src}")
				endif()
			endforeach()
			separate_arguments(opts UNIX_COMMAND "${opts}")
			add_custom_target(cppcheck_${target}
				COMMAND ${CPPCHECK_EXECUTABLE} ${opts} --language=c++ ${srcs}
				COMMENT "Checking files of ${target}" VERBATIM
			)
		else()
			message(SEND_ERROR "Unknown target ${target}")
		endif()
	endfunction()
else()
	function(cppcheck_target)
	endfunction()
endif()
