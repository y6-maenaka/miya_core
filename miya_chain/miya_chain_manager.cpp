#include "miya_chain_manager.h"


#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"
#include "./daemon/requester/requester.h"

#include "./block/block.h"
#include "./block/block_header.h"

#include "./message/message.h"
#include "./message/command/command_set.h"

#include "../ekp2p/daemon/sender/sender.h"
#include "./utxo_set/utxo_set.h"
#include "./miya_coin/local_strage_manager.h"

#include "./miya_chain_state.cpp"

namespace miya_chain
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
	// _dbManager.startWithLightMode( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC  , "../miya_chain/miya_coin/blocks/index/block_index" );
	_dbManager.startWithLightMode( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC  , "block" );

	// UTxOSetDBの簡易起動
	_utxoSetDB._toUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	_utxoSetDB._fromUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	//_dbManager.startWithLightMode( _utxoSetDB._toUTXOSetDBSBC, _utxoSetDB._fromUTXOSetDBSBC , "../miya_db/table_files/utxo/utxo" );
	_dbManager.startWithLightMode( _utxoSetDB._toUTXOSetDBSBC, _utxoSetDB._fromUTXOSetDBSBC , "utxo" );
	_utxoSet = std::make_shared<LightUTXOSet>( _utxoSetDB._toUTXOSetDBSBC , _utxoSetDB._fromUTXOSetDBSBC );// utxoSet(クライアント) のセットアップ


	// ローカルデータストア(生ブロックを直接ファイルに保存している)
	_localStrageManager._strageManager = std::make_shared<BlockLocalStrageManager>( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC );

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

std::shared_ptr<BlockLocalStrageManager> MiyaChainManager::localStrageManager()
{
	return _localStrageManager._strageManager;
}



void MiyaChainManager::__unitTest( std::vector<std::shared_ptr<block::Block>> blocks )
{
	// std::shared_ptr<unsigned char> localChainHead = _chainState->chainHead(); // これで見つからない場合は,自身のチェーンを遡って更新する

	// 仮想チェーンテスト
	/*
	struct IBDBCB initialBCB;
	initialBCB.block = _localStrageManager._strageManager->readBlock( localChainHead );
	initialBCB.status = static_cast<int>(IBDState::BlockStored);
	IBDVirtualChain vitrualChain( localChainHead, initialBCB );
	std::cout << "VirtualChain size :: " << vitrualChain.size() << "\n";
	std::cout << "----------------------------" << "\n";
	initialBCB.print();
	std::cout << "----------------------------" << "\n";


	// リクエストメッセージテスト
	MiyaChainCommand command;
	MiyaChainMSG_GETBLOCKS getblocks;
	getblocks.startHash( vitrualChain.chainHead() );
	std::cout << "---------------------------" << "\n";
	getblocks.print();
	std::cout << "---------------------------" << "\n";


	IBDHeaderFilter headerFilter( &vitrualChain ); // ヘッダフィルタ
	sleep(1);

	// HeaderFilter ヘッダー追加テスト
	std::shared_ptr<block::BlockHeader> header_0 = blocks.at(0)->headerWithSharedPtr();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
	header_0->print();
	std::cout << "verify :: " << header_0->verify() << "\n";
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
	headerFilter.add( header_0 );
	headerFilter.add( header_0 );

	sleep(1);
	std::cout << "Layer1 size() :: " << headerFilter.sizeLayer1() << "\n";
	std::cout << "Layer2 size() :: " << headerFilter.sizeLayer2() << "\n";



	// ------ HeaderFilter チェックOK -----------------

	sleep(1);
	std::cout << "HeaderFilter Check OK" << "\n";


	std::cout << "VirtualChain Size() :: " << vitrualChain.size() << "\n";
	std::cout << "--------------" << "\n";
	std::shared_ptr<unsigned char> genesisBlockHash;
	genesisBlockHash = vitrualChain.chainVector().at(0).second.blockHash();
	std::cout << "GenesisBlockHash :: ";
	for( int i=0; i<32; i++ ){
		printf("%02X", genesisBlockHash.get()[i]);
	} std::cout << "\n";
	std::cout << "--------------" << "\n";

	//ブロック本体ダウンロードスレッドテスト
	std::vector< std::thread > blockDownloadAgentThreads;
	std::cout << "BlockDownloadAgentThreads count :: " << vitrualChain.size() / 100  << "\n";
	for( int i=0; i< (vitrualChain.size() / 100) + 1; i++ )
	{
		blockDownloadAgentThreads.push_back( std::thread(IBDVirtualChain::blockDownload ,
															 &vitrualChain,
															  _requesterDaemon._toRequesterSBC ,
															  _utxoSet,
															  _localStrageManager._strageManager ) );
		blockDownloadAgentThreads.back().join();
	}

	sleep(1);
	std::cout << "VirtualChain Size() :: " << vitrualChain.size() << "\n";
	// filter : layer1チェック
	// filter : layer2チェック

	std::cout << "IBD完了" << "\n";
	*/
}




};
