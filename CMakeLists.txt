cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(pod_reflection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_EXAMPLE "Compile pod_reflection example?" ON)

get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
set(BUILD_EXAMPLE OFF)
endif(hasParent)

add_library(pod_reflection INTERFACE)

target_include_directories(pod_reflection
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	)


if(BUILD_EXAMPLE)
add_executable(pod_relf_example
	example/main.cpp
	)
	
target_link_libraries(pod_relf_example
	PRIVATE
	pod_reflection
	)
	
endif(BUILD_EXAMPLE)