cmake_minimum_required( VERSION 3.12 )

add_library( core STATIC
            core.cpp
)

add_definitions( -std=c++20 )
