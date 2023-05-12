cmake_minimum_required( VERSION 3.2 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )


add_library( MIYA_CORE STATIC

							transaction/tx_manager.cpp

							transaction/tx/tx_in.cpp
							transaction/tx/tx_out.cpp

							transaction/script/script.cpp
							transaction/script/pk_script.cpp
							transaction/script/signature_script.cpp

							transaction/txcb_table_manager/txcb_table_manager.cpp
							transaction/txcb_table_manager/txcb/txcb.cpp
							transaction/txcb_table_manager/txcb_bucket/txcb_bucket.cpp
							transaction/txcb_table_manager/txcb_table/txcb_table.cpp

						
							transaction/p2pkh/p2pkh.cpp

						
)


add_definitions( -std=c++14 )

