if(JDUTIL_CLANGTIDY)
	return()
endif()
set(JDUTIL_CLANGTIDY 1 PARENT_SCOPE)

find_program(CLANGTIDY_EXECUTABLE clang-tidy)
mark_as_advanced(CLANGTIDY_EXECUTABLE)

if(CLANGTIDY_EXECUTABLE)
	function(clang_tidy_target target)
		if(TARGET ${target})
			gather_flags_for_target(${target})

			get_target_property(srcs ${target} SOURCES)
			foreach(src ${srcs})
				if(src MATCHES ".*\.cpp$")
					set(absolute_srcs ${absolute_srcs} "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
				endif()
			endforeach()

			string(REPLACE "." "\\." source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
			add_custom_target(clang_tidy_${target} ${ARGN}
				COMMAND ${CLANGTIDY_EXECUTABLE} -header-filter="^${source_dir}/.*" ${absolute_srcs} -- ${opts}
				COMMENT "Checking files of ${target}" VERBATIM
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			)
		else()
			message(SEND_ERROR "Unknown target ${target}")
		endif()
	endfunction()
else()
	function(clang_tidy_target)
	endfunction()
endif()
