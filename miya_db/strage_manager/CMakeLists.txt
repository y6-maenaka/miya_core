cmake_minimum_required(VERSION 3.1)


add_library( STRAGE_MANAGER STATIC
						
				MMyISAM/components/index_manager/b_tree.cpp
				MMyISAM/components/index_manager/node_item_set.cpp

				MMyISAM/components/page_table/overlay_memory_manager.cpp
				MMyISAM/components/page_table/overlay_ptr.cpp
				MMyISAM/components/page_table/optr_utils.cpp					

)


add_definitions( -std=c++14 )
