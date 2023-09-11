cmake_minimum_required( VERSION 3.2 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )


add_library( MIYA_CHAIN STATIC

							transaction/tx_manager.cpp

							transaction/tx/tx_in.cpp
							transaction/tx/tx_out.cpp

							transaction/script/script.cpp
							transaction/script/pk_script.cpp
							transaction/script/signature_script.cpp

							transaction_pool/transaction_pool.cpp
							transaction_pool/txcb.cpp
							transaction_pool/txcb_bucket.cpp
							transaction_pool/txcb_table.cpp
							transaction_pool/provisional_utxo_cache/provisional_utxo_cache.cpp

							daemon/transaction_validator/transaction_validator.cpp
							daemon/block_validator/block_validator.cpp
							daemon/message_broker/message_broker.cpp
							daemon/request_responder/request_responder.cpp
									
							block_assemble_agent/block_accemble_agent.cpp

							message/protocol_message/block_data_msg.cpp
							message/protocol_message/block_header_msg.cpp
							message/message.cpp

							#transaction/txcb_table_manager/txcb_table_manager.cpp
							#transaction/txcb_table_manager/txcb/txcb.cpp
							#transaction/txcb_table_manager/txcb_bucket/txcb_bucket.cpp
							#transaction/txcb_table_manager/txcb_table/txcb_table.cpp

						
							transaction/p2pkh/p2pkh.cpp

						
)


add_definitions( -std=c++14 )

