cmake_minimum_required( VERSION 3.12 )

add_library( MINING_MANAGER STATIC
  mining_manager.cpp
)

add_definitions( -std=c++20 )
