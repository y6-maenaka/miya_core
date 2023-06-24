cmake_minimum_required(VERSION 3.1)

add_library( STRAGE_MANAGER STATIC
						

				MMyISAM/components/index_manager/b_tree.cpp
				MMyISAM/components/index_manager/node_item_set.cpp
				
)


add_definitions( -std=c++14 )
