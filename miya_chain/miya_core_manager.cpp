#include "miya_core_manager.h"


#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"
#include "./daemon/requester/requester.h"

#include "./block_chain_manager/IBD.h"

namespace miya_chain
{








int MiyaChainManager::init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC )
{

	// はじめに全てのデータベースを起動する
	

	// BlockIndexDBの簡易起動
	_blockIndexDB._toBlockIndexDBSBC = std::make_shared<StreamBufferContainer>();
	_blockIndexDB._fromBlockIndexDBSBC = std::make_shared<StreamBufferContainer>();
	_dbManager.startWithLightMode( _blockIndexDB._toBlockIndexDBSBC , _blockIndexDB._fromBlockIndexDBSBC  , "../miya_chain/miya_coin/blocks/index/block_index" );

	// UTxOSetDBの簡易起動
	_utxoSetDB._toUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	_utxoSetDB._fromUTXOSetDBSBC = std::make_shared<StreamBufferContainer>();
	_dbManager.startWithLightMode( _utxoSetDB._toUTXOSetDBSBC, _utxoSetDB._fromUTXOSetDBSBC , "../miya_db/table_files/test/test" );






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



bool MiyaChainManager::startIBD()
{

	IBDManager manager;
	manager.start( this->chainState() , _requesterDaemon._toRequesterSBC  );
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


const std::shared_ptr<MiyaChainState> MiyaChainManager::chainState()
{
	return std::make_shared<MiyaChainState>(_chainState);
}

};
