cmake_minimum_required( VERSION 3.2 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )

add_library( CONTROL_INTERFACE STATIC 
							control_interface.cpp
)


add_definitions( -std=c++20 )

