cmake_minimum_required( VERSION 3.12 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )



add_library( MIYA_CHAIN STATIC

							miya_chain_manager.cpp
							miya_chain_state.cpp
							IBD.cpp

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
							block/block_c_iterator.cpp
							block/block_header.cpp

							transaction_pool/transaction_pool.cpp
							transaction_pool/txcb.cpp
							transaction_pool/txcb_bucket.cpp
							transaction_pool/txcb_table.cpp
							#transaction_pool/provisional_utxo_cache/provisional_utxo_cache.cpp
							transaction_pool/unit_test.cpp

							daemon/transaction_validator/transaction_validator.cpp
							daemon/block_validator/block_validator.cpp
							daemon/broker/broker.cpp
							daemon/requester/requester.cpp
							daemon/chain_maintainer/chain_maintainer.cpp


							#message/protocol_message/block_data_msg.cpp
							#message/protocol_message/block_header_msg.cpp
							message/message.cpp
							message/__test.cpp
							message/command/inv/inv.cpp
							message/command/getblocks.cpp
							message/command/headers.cpp
							message/command/notfound.cpp
							message/command/getdata.cpp
							message/command/block.cpp

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
							miya_coin/block_container.cpp
							miya_coin/undo_container.cpp

							virtual_chain/virtual_chain.cpp
							virtual_chain/bd_filter.cpp
							virtual_chain/block_hash_as_key.cpp
							virtual_chain/prev_block_hash_as_key.cpp

)



add_definitions( -std=c++20 -w )
