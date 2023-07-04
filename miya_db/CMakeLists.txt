cmake_minimum_required( VERSION 3.2 )

add_library( MIYA_DB STATIC 

				strage_manager/MMyISAM/components/index_manager/b_tree.cpp
				strage_manager/MMyISAM/components/index_manager/node_item_set.cpp


				strage_manager/MMyISAM/components/page_table/page_table.cpp
				strage_manager/MMyISAM/components/page_table/overlay_ptr.cpp
				strage_manager/MMyISAM/components/page_table/overlay_ptr_resolver.cpp

				strage_manager/MMyISAM/components/page_table/cache_manager/cache_table.cpp
				strage_manager/MMyISAM/components/page_table/cache_manager/cache_table_LRU.cpp
				strage_manager/MMyISAM/components/page_table/cache_manager/mapper/mapper.cpp

)


add_definitions( -std=c++14 )

