cmake_minimum_required( VERSION 3.12 )

add_library( TEST_MIYA_DB STATIC 
			
			  db_append_normal_p1.cpp	
			  db_remove_normal_p1.cpp

			  db_safe_p1.cpp
			  db_safe_p2.cpp
			  db_safe_p3.cpp
)

add_definitions( -std=c++20 -w )
