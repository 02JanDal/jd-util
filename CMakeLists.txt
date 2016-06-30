cmake_minimum_required(VERSION 3.1)

project(jd-util)

find_package(Qt5 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)

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
	functional/Tap.h
)
prepend_to_list(SRC src/jd-util/)

add_library(jd-util ${SRC})
target_link_libraries(jd-util Qt5::Core)
target_include_directories(jd-util PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS jd-util EXPORT jd-util DESTINATION lib)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(EXPORT jd-util NAMESPACE jd_ DESTINATION lib/cmake)

enable_testing()
add_subdirectory(test)
