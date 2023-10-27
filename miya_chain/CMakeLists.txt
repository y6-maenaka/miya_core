cmake_minimum_required( VERSION 3.2 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )



add_library( MIYA_CHAIN STATIC
							
							miya_core_manager.cpp
							miya_chain_state.cpp

							transaction/tx_manager.cpp

							transaction/tx/tx_in.cpp
							transaction/tx/tx_out.cpp
							transaction/tx/prev_out.cpp	

							transaction/script/script.cpp
							transaction/script/script_validator.cpp
							transaction/script/pk_script.cpp
							transaction/script/signature_script.cpp
							transaction/tx/tx.cpp

							block/block.cpp

							transaction_pool/transaction_pool.cpp
							transaction_pool/txcb.cpp
							transaction_pool/txcb_bucket.cpp
							transaction_pool/txcb_table.cpp
							#transaction_pool/provisional_utxo_cache/provisional_utxo_cache.cpp
							transaction_pool/unit_test.cpp

							daemon/transaction_validator/transaction_validator.cpp
							daemon/block_validator/block_validator.cpp
							daemon/broker/broker.cpp
									
							block_assemble_agent/block_accemble_agent.cpp

							#message/protocol_message/block_data_msg.cpp
							#message/protocol_message/block_header_msg.cpp
							message/message.cpp
							message/command/inv/inv.cpp
							message/command/getblocks.cpp

							transaction/p2pkh/p2pkh.cpp
							transaction/coinbase/coinbase.cpp
							transaction/coinbase/coinbase_tx_in.cpp

							utxo_set/utxo.cpp
							utxo_set/utxo_set.cpp

							block_chain_manager/block_chain_manager.cpp
							#block_chain_manager/block_local_strage_manager/transaction_store/transaction_store.cpp

							mining/simple_mining.cpp
							
							miya_coin/local_strage_manager.cpp
							miya_coin/local_file_controller.cpp

)



add_definitions( -std=c++17 -w )

