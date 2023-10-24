#include "chain_manager.h"


#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"

namespace miya_chain
{




int MiyaChainManager::init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC )
{
	_toEKP2PBrokerSBC = toEKP2PBrokerSBC;

	// ブローカーの起動
	_brokerDaemon._toBrokerSBC = std::make_shared<StreamBufferContainer>();

  
	_brokerDaemon._broker = std::make_shared<MiyaChainMessageBrocker>( _brokerDaemon._toBrokerSBC , _toEKP2PBrokerSBC );

	std::cout << "MiyaChainManager initialize successfully done" << "\n";
}



int MiyaChainManager::start()
{
	_brokerDaemon._broker->start();
}



std::shared_ptr<StreamBufferContainer> MiyaChainManager::toBrokerSBC()
{
	return _brokerDaemon._toBrokerSBC;
}





};
