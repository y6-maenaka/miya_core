cmake_minimum_required( VERSION 3.12 )

add_library( TEST_EKP2P STATIC 
			  nat_traversal.cpp	
)


add_definitions( -std=c++20 -w )
