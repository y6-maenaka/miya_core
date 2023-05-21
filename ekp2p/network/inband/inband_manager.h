#ifndef AC403C7C_AE72_4750_AEC7_2DB1FE59F95F
#define AC403C7C_AE72_4750_AEC7_2DB1FE59F95F

#include <iostream>
#include <pthread.h>
#include <map>
#include <functional>
#include <errno.h>

#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

namespace ekp2p{

class SocketManager;
class ReceiveHandler;
class KRoutingTable;
struct MSGHeader;
class EKP2PMSG;
struct KTag;


/*
class NetworkManager{
private: static UDPInbandManager _udpInbandManager;
};
*/



void DefaultMessageHandler( void *arg , EKP2PMSG *msg );





// 単体では使用できない
class BaseInbandManager{

protected:

	//MiddleBuffer *_middleBuffer; // 全ての受信スレッドはMSGをこのバッファに投入する // 監視はKademlia側で行う
	SocketManager *_socketManager;
	
	// 受信したメッセージがこのアプリケーションのものか
	static bool ValidateMSGHeader( struct MSGHeader *header , unsigned int allowedDataSize = 100000 );

	void *_handlerArg;

public:
	SocketManager *socketManager(); // getter 

	std::function< void(void*,EKP2PMSG*)> _messageHandler;

	BaseInbandManager();
	~BaseInbandManager();

  // マイナスの値だと正規のデータセグメントではない	
	static MSGHeader *GetOutMSGHeader( int sock );

	void handlerArg( void* arg ); // setter
	void* handlerArg(); // getter
};






class UDPInbandManager : public BaseInbandManager {

public:

	MSGHeader *GetOutMSGHeader( int sock );

};



class TCPInbandManager : public BaseInbandManager {

public:
	// pthread_t start();	
	// friend void* TCPHandlerCaller();

	// static MSGHeader *UDP_GetOutMSGHeader( int sock )
};






class InbandNetworkManager{

private:
	
	std::vector< std::pair<SocketManager*, std::thread> > _activeThreadList;
	// socketManagerを強制的にcloseすればexitするように処理する



public:
	//friend void TCPPreHandler();

	bool start( unsigned short targetPort , int type );

	
};











} // close ekp2p namespace 



#endif // AC403C7C_AE72_4750_AEC7_2DB1FE59F95F

