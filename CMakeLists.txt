cmake_minimum_required(VERSION 3.5)

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

option(USE_IWYU "Wether to enable include-what-you-use or not" OFF)
if(USE_IWYU)
	find_package(IWYU)
endif()

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

	MultiLevelModel.h
	MultiLevelModel.cpp

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
	-isystem $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	-isystem $<INSTALL_INTERFACE:include>
)
set_target_properties(jd-util PROPERTIES OUTPUT_NAME jd_util)
clean_qt(jd-util)

set(GUI_SRC
	UndoUtil.h
	UndoUtil.cpp
)
prepend_to_list(GUI_SRC src/jd-util-gui/)
add_library(jd-util-gui ${GUI_SRC})
target_link_libraries(jd-util-gui Qt5::Core Qt5::Gui Qt5::Widgets jd-util)
target_include_directories(jd-util-gui PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include>
)
set_target_properties(jd-util-gui PROPERTIES OUTPUT_NAME jd_util_gui)
clean_qt(jd-util-gui)

install(TARGETS jd-util jd-util-gui EXPORT jd-util DESTINATION lib)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(EXPORT jd-util NAMESPACE jd_ DESTINATION lib/cmake)
install(DIRECTORY cmake/ DESTINATION lib/cmake/jd-util)

include(CppCheck)
include(ClangTidy)
cppcheck_target(jd-util)
clang_tidy_target(jd-util)
cppcheck_target(jd-util-gui)
clang_tidy_target(jd-util-gui)

enable_testing()
add_subdirectory(test)
