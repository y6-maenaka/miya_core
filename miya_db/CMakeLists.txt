cmake_minimum_required( VERSION 3.2 )

add_library( MIYA_DB STATIC 

				strage_manager/MMyISAM/components/index_manager/b_tree.cpp
				strage_manager/MMyISAM/components/index_manager/node_item_set.cpp
)


add_definitions( -std=c++14 )

