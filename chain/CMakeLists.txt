cmake_minimum_required( VERSION 3.12 )



add_library( CHAIN STATIC

	  chain.cpp
	  chain_manager.cpp
	  chain_state.cpp
	  IBD.cpp

	  transaction/tx_manager.cpp

	  transaction/tx/tx_in.cpp
	  transaction/tx/tx_out.cpp
	  transaction/tx/prev_out.cpp

	  transaction/script/script.cpp
	  transaction/script/script_validator.cpp
	  transaction/script/pk_script.cpp
	  transaction/script/signature_script.cpp

	  block/block.cpp
	  block/block_header.cpp

	  block_chain_iterator/block_chain_iterator.cpp

	  transaction_pool/transaction_pool.cpp
	  transaction_pool/txcb.cpp
	  transaction_pool/txcb_bucket.cpp
	  transaction_pool/txcb_table.cpp
	  #transaction_pool/provisional_utxo_cache/provisional_utxo_cache.cpp
	  transaction_pool/unit_test.cpp

	  # daemon/transaction_validator/transaction_validator.cpp
	  # daemon/block_validator/block_validator.cpp
	  # daemon/broker/broker.cpp
	  # daemon/requester/requester.cpp
	  # daemon/chain_maintainer/chain_maintainer.cpp

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

	  chain_sync_manager/chain_sync_manager.cpp
	  chain_sync_manager/bd_filter.cpp
	  chain_sync_manager/block_hash_as_key.cpp
	  chain_sync_manager/prev_block_hash_as_key.cpp
	  chain_sync_manager/BDBCB.cpp
	  chain_sync_manager/virtual_header_subchain.cpp
	  chain_sync_manager/virtual_header_sync_manager.cpp
	  chain_sync_manager/virtual_block_sync_manager.cpp
	  chain_sync_manager/virtual_block.cpp
	  chain_sync_manager/block_hash_pool.cpp
	  chain_sync_manager/block_header_pool.cpp

	  block_assembler.cpp
)

target_include_directories( CHAIN PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

get_property(include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message("chain Included Directories:")
foreach(dir ${include_dirs})
      message("  ${dir}")
endforeach()

add_definitions( -std=c++20 -w )
