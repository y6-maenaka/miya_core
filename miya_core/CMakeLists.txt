cmake_minimum_required( VERSION 3.12 )

add_library( MIYA_CORE STATIC
            miya_core.cpp
)

add_definitions( -std=c++20 )
