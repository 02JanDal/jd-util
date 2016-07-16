find_library(AVAHI_LIBRARY_COMMON NAMES avahi-common)
find_library(AVAHI_LIBRARY_CLIENT NAMES avahi-client)
find_library(AVAHI_LIBRARY_CORE NAMES avahi-core)
find_path(AVAHI_INCLUDE_DIR avahi-client/publish.h)

# try to determine the version
find_program(AVAHI_DAEMON_EXECUTABLE avahi-daemon)
if(AVAHI_DAEMON_EXECUTABLE)
	execute_process(COMMAND ${AVAHI_DAEMON_EXECUTABLE} --version OUTPUT_VARIABLE out)
	string(REPLACE "avahi-daemon " "" out "${out}")
	string(REPLACE "\n" "" out "${out}")
	set(AVAHI_VERSION ${out})
else()
	message(WARNING "Unable to determine avahi version")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Avahi DEFAULT_MSG
	AVAHI_VERSION AVAHI_LIBRARY_COMMON AVAHI_LIBRARY_CLIENT AVAHI_LIBRARY_CORE AVAHI_INCLUDE_DIR
)

if (AVAHI_FOUND)
	add_library(avahi-common SHARED IMPORTED)
	set_target_properties(avahi-common PROPERTIES
		IMPORTED_LOCATION ${AVAHI_LIBRARY_COMMON}
		INTERFACE_INCLUDE_DIRECTORIES ${AVAHI_INCLUDE_DIR}
	)
	add_library(avahi-client SHARED IMPORTED)
	set_target_properties(avahi-client PROPERTIES
		IMPORTED_LOCATION ${AVAHI_LIBRARY_CLIENT}
		INTERFACE_INCLUDE_DIRECTORIES ${AVAHI_INCLUDE_DIR}
		INTERFACE_LINK_LIBRARIES avahi-common
	)
	add_library(avahi-core SHARED IMPORTED)
	set_target_properties(avahi-core PROPERTIES
		IMPORTED_LOCATION ${AVAHI_LIBRARY_CORE}
		INTERFACE_INCLUDE_DIRECTORIES ${AVAHI_INCLUDE_DIR}
		INTERFACE_LINK_LIBRARIES avahi-common
	)
endif()

mark_as_advanced(AVAHI_LIBRARY_COMMON AVAHI_LIBRARY_CLIENT AVAHI_LIBRARY_CORE AVAHI_INCLUDE_DIR)
