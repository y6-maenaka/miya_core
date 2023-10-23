#ifndef A477CEFA_5DDF_43C5_8088_14D69EEC7AF3
#define A477CEFA_5DDF_43C5_8088_14D69EEC7AF3



#include <iostream>
#include <memory>
#include <thread>
#include <vector>





class StreamBuffer;
class StreamBufferContainer;



struct AllowedProtocolSet;


namespace ekp2p
{



constexpr unsigned short MAX_PROTOCOL = 10;


class EKP2PBroker
{
	std::shared_ptr<StreamBufferContainer> _incomingSB;
	std::vector<std::thread::id> _activeSenderThreadIDVector; // 念の為管理しておく

	std::shared_ptr<StreamBufferContainer> _toRoutingTableManagerSBC;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {}; // メッセージプロトコルに合致したSBにメッセージを流す
	bool _allowedProtocolSet[MAX_PROTOCOL];


public:
	EKP2PBroker( std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> _toRoutingTableUpdatorSBC );
	int start( bool requiresRouting = true );
	int forwardingDestination( std::shared_ptr<StreamBufferContainer> sbc , unsigned short destination );
};



};



#endif// A477CEFA_5DDF_43C5_8088_14D69EEC7AF3

