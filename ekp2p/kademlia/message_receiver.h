#ifndef A9C4D0E9_594A_4E10_A55E_9BDB450B63DA
#define A9C4D0E9_594A_4E10_A55E_9BDB450B63DA



namespace ekp2p{



class SmartMiddleBuffer;
class SocketManager;





class MessageReceiver // 基本的にスレッドで起動されう
{

private:
	SmartMiddleBuffer *_middleBuffer;
	SocketManager *_socketManager;
	


public:
	void start();

	static unsigned int payload( void *rawEKP2PMSG , unsigned char** ret );

};


};


#endif // 8AF85C80_D2B6_4809_BC2C_1499249D045F


