if(JDUTIL_HELPERS)
	return()
endif()
set(JDUTIL_HELPERS 1)

macro(prepend_to_list variable str)
	set(out )
	foreach(item ${${variable}})
		list(APPEND out ${str}${item})
	endforeach()
	set(${variable} ${out})
endmacro()

macro(add_ui_files variable basename)
	set(${variable} ${${variable}}
		${basename}.h
		${basename}.cpp
		${basename}.ui
	)
endmacro()
