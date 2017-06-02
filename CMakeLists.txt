cmake_minimum_required(VERSION 3.1)

project(jd-util)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(JDUTIL_BUILD_CLAZY ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
if(NOT CMAKE_SOURCE_DIR STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Git REQUIRED)
find_package(Catch REQUIRED)
find_package(Nonius REQUIRED)
find_package(Clazy REQUIRED)

include(Helpers)

set(SRC
	BaseConfigObject.h
	BaseConfigObject.cpp
	Exception.h
	Exception.cpp
	FileSystem.h
	FileSystem.cpp
	Json.h
	Json.cpp
	TermUtil.h
	TermUtil.cpp
	Formatting.h
	Compiler.h
	Util.h
	Util.cpp
	Backtrace.h
	Backtrace.cpp
	Logging.h
	Logging.cpp
	Introspection.h
	Introspection.cpp
	Inflector.h
	Inflector.cpp

	CommandLineParser.h
	CommandLineParser_p.h
	CommandLineParser.cpp

	Functional.h
	functional/Base.h
	functional/Collection.h
	functional/ContainerTraits.h
	functional/Each.h
	functional/Eval.h
	functional/Filter.h
	functional/Functions.h
	functional/FunctionTraits.h
	functional/Map.h
	functional/Sort.h
	functional/Tap.h
)
prepend_to_list(SRC src/jd-util/)

add_library(jd-util ${SRC})
target_link_libraries(jd-util Qt5::Core)
target_include_directories(jd-util PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include>
)
clean_qt(jd-util)

install(TARGETS jd-util EXPORT jd-util DESTINATION lib)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(EXPORT jd-util NAMESPACE jd_ DESTINATION lib/cmake)
install(DIRECTORY cmake/ DESTINATION lib/cmake/jd-util)

include(CppCheck)
cppcheck_target(jd-util)

enable_testing()
add_subdirectory(test)
