#include "chain_manager.h"
#include "chain_manager.hpp"


namespace chain
{


int MiyaChainManager::init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC )
{
	// はじめに全てのデータベースを起動する
	/*
	std::filesystem::path currentPath = std::filesystem::current_path();
	std::cout << currentPath << "\n";
	*/

	// BlockIndexDBの簡易起動
	_blockIndexDB._toBlockIndexDBSBC = std::make_shared<StreamBufferContainer>();
	_blockIndexDB._fromBlockIndexDBSBC = std::make_shared<StreamBufferContainer>();
	// _dbManager.startWithLightMode( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC  , "../chain/miya_coin/blocks/index/block_index" );
	_dbManager.startWithLightMode( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC  , "block" );

	// UTxOSetDBの簡易起動
	_utxoSetDB._toUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	_utxoSetDB._fromUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	//_dbManager.startWithLightMode( _utxoSetDB._toUTXOSetDBSBC, _utxoSetDB._fromUTXOSetDBSBC , "../miya_db/table_files/utxo/utxo" );
	_dbManager.startWithLightMode( _utxoSetDB._toUTXOSetDBSBC, _utxoSetDB._fromUTXOSetDBSBC , "utxo" );
	_utxoSet = std::make_shared<LightUTXOSet>( _utxoSetDB._toUTXOSetDBSBC , _utxoSetDB._fromUTXOSetDBSBC );// utxoSet(クライアント) のセットアップ


	// ローカルデータストア(生ブロックを直接ファイルに保存している)
	_localStrageManager._strageManager = std::make_shared<BlockLocalStrage>( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC );

	// ChainStateのセットアップ
	_chainState = std::make_shared<MiyaChainState>( _localStrageManager._strageManager );

	_toEKP2PBrokerSBC = toEKP2PBrokerSBC;

	// ブローカーの起動
	_brokerDaemon._toBrokerSBC = std::make_shared<StreamBufferContainer>();
	_requesterDaemon._toRequesterSBC = std::make_shared<StreamBufferContainer>();

	_brokerDaemon._broker = std::make_shared<MiyaChainBrocker>( _brokerDaemon._toBrokerSBC , _toEKP2PBrokerSBC );
	_requesterDaemon._requester = std::make_shared<MiyaChainRequester>( _requesterDaemon._toRequesterSBC , _brokerDaemon._toBrokerSBC );


	std::cout << "MiyaChainManager initialize successfully done" << "\n";
}



int MiyaChainManager::start()
{
	_brokerDaemon._broker->start();
	_requesterDaemon._requester->start();

	std::cout << "MiyaChainManager Started" << "\n";
}



std::shared_ptr<StreamBufferContainer> MiyaChainManager::toBrokerSBC()
{
	return _brokerDaemon._toBrokerSBC;
}


std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer> > MiyaChainManager::blockIndexDBSBCPair()
{
	return std::make_pair( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC );
}

std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer>> MiyaChainManager::utxoSetDBSBCPair()
{
	return std::make_pair( _utxoSetDB._toUTXOSetDBSBC , _utxoSetDB._fromUTXOSetDBSBC );
}

std::shared_ptr<MiyaChainState> MiyaChainManager::chainState()
{
	return _chainState;
}

std::shared_ptr<LightUTXOSet> MiyaChainManager::utxoSet()
{
	return _utxoSet;
}

std::shared_ptr<BlockLocalStrage> MiyaChainManager::localStrageManager()
{
	return _localStrageManager._strageManager;
}







chain_manager::chain_manager( io_context &io_ctx, class core_context &core_ctx, class BlockLocalStrage &block_strage, std::string path_to_chainstate_sr, std::function<void(void)> on_chain_update ) : 
  _block_strage( block_strage )
  // , _local_chain( path_to_chainstate_sr, _block_strage )
  , _io_ctx( io_ctx )
  , _refresh_timer( io_ctx )
{
  return;
}

/* class local_chain& chain_manager::get_local_chain()
{
  // return _local_chain;
} */


};
