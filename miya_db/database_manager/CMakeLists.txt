cmake_minimum_required( VERSION 3.1 )

add_library( DATABASE_MANAGER STATIC 
							
						middle_buffer/middle_buffre.cpp	
)


add_definitions( -std=c++14 )
