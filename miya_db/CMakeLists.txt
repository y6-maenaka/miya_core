cmake_minimum_required( VERSION 3.12 )


add_library( MIYA_DB STATIC


				strage_manager/MMyISAM/components/value_store_manager/value_store_manager.cpp

				strage_manager/MMyISAM/MMyISAM.cpp
				strage_manager/unified_storage_manager/unified_storage_manager.cpp


				strage_manager/MMyISAM/components/index_manager/btree.cpp
				strage_manager/MMyISAM/components/index_manager/o_node.cpp
				strage_manager/MMyISAM/components/index_manager/index_manager.cpp
				strage_manager/MMyISAM/components/index_manager/normal_index_manager.cpp

				strage_manager/MMyISAM/components/page_table/page_table.cpp
				strage_manager/MMyISAM/components/page_table/overlay_ptr.cpp
				strage_manager/MMyISAM/components/page_table/optr_utils.cpp
				strage_manager/MMyISAM/components/page_table/overlay_memory_manager.cpp
				strage_manager/MMyISAM/components/page_table/overlay_memory_allocator.cpp
				strage_manager/MMyISAM/components/page_table/control_block.cpp
				strage_manager/MMyISAM/components/page_table/meta_block.cpp

				strage_manager/MMyISAM/components/page_table/unit_test.cpp

				strage_manager/MMyISAM/components/page_table/cache_manager/cache_table.cpp
				strage_manager/MMyISAM/components/page_table/cache_manager/cache_table_LRU.cpp
				strage_manager/MMyISAM/components/page_table/cache_manager/mapper/mapper.cpp
				strage_manager/MMyISAM/safe_mode_manager/safe_index_manager.cpp
				strage_manager/MMyISAM/safe_mode_manager/safe_o_node.cpp
				strage_manager/MMyISAM/safe_mode_manager/safe_btree.cpp

				strage_manager/MMyISAM/components/.unit_test.cpp

				miya_db/database_manager.cpp
				# miya_db_client/miya_db_client.cpp
				miya_db/query_parser/query_parser.cpp
				miya_db/query_context/query_context.cpp
)

target_include_directories( MIYA_DB PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_definitions( -std=c++20 -w )
