cmake_minimum_required(VERSION 3.1)

project(jd-util)

find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Git REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)

include(Helpers)

include(ExternalProject)
ExternalProject_Add(catch_dl
	PREFIX ${CMAKE_BINARY_DIR}/catch
	GIT_REPOSITORY https://github.com/philsquared/Catch.git
	TIMEOUT 10
	UPDATE_COMMAND ${GIT_EXECUTABLE} pull
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
)
ExternalProject_Get_Property(catch_dl SOURCE_DIR)
add_library(catch STATIC IMPORTED)
set_target_properties(catch PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${SOURCE_DIR}/single_include)
add_dependencies(catch catch_dl)

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
install(DIRECTORY cmake/ DESTINATION lib/cmake/jd-util)

enable_testing()
add_subdirectory(test)
