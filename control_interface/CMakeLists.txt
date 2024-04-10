cmake_minimum_required( VERSION 3.12 )

add_library( CONTROL_INTERFACE STATIC 
							control_interface.cpp
)


target_include_directories( CONTROL_INTERFACE PUBLIC 
  ${OPENSSL_INCLUDE_DIR}
)



add_definitions( -std=c++20 )

