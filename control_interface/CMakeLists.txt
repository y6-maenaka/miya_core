cmake_minimum_required( VERSION 3.2 )


add_library( CONTROL_INTERFACE STATIC 
							control_interface.cpp
)


add_definitions( -std=c++14 )

