#ifndef E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17
#define E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17


#include <iostream>
#include <memory>
#include <thread>





class StreamBuffer;
class StreamBufferContainer;





namespace ekp2p
{




constexpr unsigned short MAX_PROTOCOL = 10;


class SocketManager;
struct EKP2PMessage;
struct AllowedProtocolSet;







class Receiver // 基本的にスレッドで起動されう
{

private:

	// SocketManager *_socketManager;
	std::shared_ptr<SocketManager> _listeningSocketManager;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {}; // メッセージプロトコルに合致したSBにメッセージを流す
	
	std::vector<std::thread::id> _activeSenderThreadIDVector; // 念の為管理しておく
	bool _allowedProtocolSet[MAX_PROTOCOL] = {true};

public:
	// Receiver( std::shared_ptr<StreamBufferContainer> incomingSB );
	Receiver( std::shared_ptr<SocketManager> target );

	void start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	std::shared_ptr<EKP2PMessage> parseRawEKP2PMessage( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ); 
	// static unsigned int payload( void *rawEKP2PMSG , unsigned char** ret );
};




};

#endif // E1BD1EF2_8896_4EB3_AD30_3CC6FF304B17



