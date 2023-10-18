#ifndef A9C4D0E9_594A_4E10_A55E_9BDB450B63DA
#define A9C4D0E9_594A_4E10_A55E_9BDB450B63DA



#include <map>
#include <vector>
#include <array>
#include <memory>

class StreamBuffer;
class StreamBufferContainer;




namespace ekp2p{



class SocketManager;


constexpr unsigned short MAX_PROTOCOL = 20;




class MessageReceiver // 基本的にスレッドで起動されう
{

private:

	SocketManager *_socketManager;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {}; // メッセージプロトコルに合致したSBにメッセージを流す

public:
	void start();

	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	static unsigned int payload( void *rawEKP2PMSG , unsigned char** ret );

	MessageReceiver( SocketManager* target );
};


};


#endif // 8AF85C80_D2B6_4809_BC2C_1499249D045F



