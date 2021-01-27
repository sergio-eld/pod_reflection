cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(pod_reflection)

option(BUILD_EXAMPLE "Compile pod_reflection example?" ON)

get_directory_property(hasParent PARENT_DIRECTORY)

if (hasParent)
    set(BUILD_EXAMPLE OFF)
endif (hasParent)

add_library(pod_reflection INTERFACE)

target_include_directories(pod_reflection INTERFACE include/
        )

add_library(eld::pod_reflection ALIAS pod_reflection)

if (BUILD_EXAMPLE)

	add_executable(sb.reflection_11 example/main.cpp)
	target_link_libraries(sb.reflection_11 PRIVATE pod_reflection)
	set_target_properties(sb.reflection_11
			PROPERTIES
			CXX_STANDARD 11
			CXX_STANDARD_REQUIRED TRUE
			)

	add_executable(sb.reflection_14 example/main.cpp)
	target_link_libraries(sb.reflection_14 PRIVATE pod_reflection)
	set_target_properties(sb.reflection_14
			PROPERTIES
			CXX_STANDARD 14
			CXX_STANDARD_REQUIRED TRUE
			)

    add_executable(test.reflection_11 tests/test.cpp)
    target_link_libraries(test.reflection_11 PRIVATE pod_reflection)
    set_target_properties(test.reflection_11
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED TRUE
            )

    add_executable(test.reflection_14 tests/test.cpp)
    target_link_libraries(test.reflection_14 PRIVATE pod_reflection)
    set_target_properties(test.reflection_14
            PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED TRUE
            )

endif (BUILD_EXAMPLE)