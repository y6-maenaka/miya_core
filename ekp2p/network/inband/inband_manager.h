#ifndef AC403C7C_AE72_4750_AEC7_2DB1FE59F95F
#define AC403C7C_AE72_4750_AEC7_2DB1FE59F95F

#include <iostream>
#include <pthread.h>
#include <map>
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
struct KTag;

/*
class NetworkManager{
private: static UDPInbandManager _udpInbandManager;
};
*/





// threadに複数の引数を渡すため
struct ReceiveThreadArgs{
	SocketManager* socketManager = NULL;
	ReceiveHandler *receiveHandler = NULL;		
	KRoutingTable *kRoutingTable = NULL;
};



// 単体では使用できない
class InbandManager{

protected:

	SocketManager *_socketManager;
	ReceiveHandler* _receiveHandler;

	std::map< short int , pthread_t* > _activeThreadIDList;

	friend void* handlerCaller( void *arg );

	// 受信したメッセージがこのアプリケーションのものか
	static bool ValidateMSGHeader( struct MSGHeader *header , unsigned int allowedDataSize = 100000 );


public:

	InbandManager();
	~InbandManager();

	void receiveHandler( ReceiveHandler* receiveHandler ); // setter

	// socketの作成と指定ポートのBind / receiveスレッドの起動
	// pthread_t* start(  KRoutingTable* mainKRoutingTable = NULL , short int targetPort = 8080 );

	/* stop : 失敗すると非0をリターンする */
	int stop( short int targetPort );
	int stop( pthread_t targetThreadID );

  // マイナスの値だと正規のデータセグメントではない	
	static MSGHeader *GetOutMSGHeader( int sock );
};






class UDPInbandManager : public InbandManager {

public:

	pthread_t* start(  KRoutingTable* mainKRoutingTable = NULL , short int targetPort = 8080 );
	friend void* UDP_HandlerCaller();

	static MSGHeader *UDP_GetOutMSGHeader( int sock );
};



class TCPInbandManager : public InbandManager {

public:
	// pthread_t start();	
	// friend void* TCPHandlerCaller();

	// static MSGHeader *UDP_GetOutMSGHeader( int sock )
};







} // close ekp2p namespace 



#endif // AC403C7C_AE72_4750_AEC7_2DB1FE59F95F

