cmake_minimum_required( VERSION 3.12 )

add_library( core STATIC
            core.cpp
			core_context.cpp
			net_processing.cpp
)

add_definitions( -std=c++20 )
