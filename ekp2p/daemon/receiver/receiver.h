#ifndef E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17
#define E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17


#include <iostream>
#include <memory>
#include <thread>
#include <vector>





class StreamBuffer;
class StreamBufferContainer;





namespace ekp2p
{






class SocketManager;
struct EKP2PMessage;







class EKP2PReceiver // 基本的にスレッドで起動されう
{

private:
	std::shared_ptr<SocketManager> _hostSocketManager;
	// SocketManager *_socketManager;
	//std::shared_ptr<SocketManager> _listeningSocketManager;
	std::shared_ptr<StreamBufferContainer> _toRoutingTableUpdatorSBC; // これはsbHubとは分けておく
	std::shared_ptr<StreamBufferContainer> _toBrokerSBC; // 念の為
	// std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {nullptr}; // メッセージプロトコルに合致したSBにメッセージを流す
	
	std::vector<std::thread::id> _activeSenderThreadIDVector; // 念の為管理しておく

public:
	// Receiver( std::shared_ptr<StreamBufferContainer> incomingSB );
	EKP2PReceiver( std::shared_ptr<SocketManager> target , std::shared_ptr<StreamBufferContainer> toBrokerSBC );

	int start();
	void toRoutingTableUpdatorSBC( std::shared_ptr<StreamBufferContainer> sbc );

	std::shared_ptr<EKP2PMessage> parseRawEKP2PMessage( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ); 
	// static unsigned int payload( void *rawEKP2PMSG , unsigned char** ret );
};




};

#endif // E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17



