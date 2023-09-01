#ifndef A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C
#define A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C


#include <iostream>
#include <arpa/inet.h>
#include <sys/_types/_int64_t.h>
#include <sys/socket.h>

#include <memory>


constexpr unsigned short DEFAULT_BIND_PORT = 8080;
constexpr int DEFAULT_FIND_NODE_ROUTINE_TIMEOUT = 10;



namespace ekp2p{

class InbandNetworkManager;
class KRoutingTable;
class SocketManager;
class SmartMiddleBuffer;

class KNodeAddr;
class KHostNode;
class KClientNode;





constexpr unsigned short _PROTOCOL_NAT_ = 3;





class EKP2P // 基本的にNAT超え後はそのSocketManagerを使い回し続ける必要がありそう
{

private:

	InbandNetworkManager *_inbandManager; // tcp,udp 兼用
	
	KRoutingTable *_kRoutingTable = nullptr;

	SocketManager *_mainSocketManager;
	std::shared_ptr<KHostNode> _mainNode;



public:
	//EKP2P( KRoutingTable *baseKRoutingTable = nullptr );
	EKP2P();
	 
	void init(); // KRoutingTableを使うのであれば必須 自身のグローバルアドレスを取得する
	bool collectStartUpNodes( SocketManager *baseSocketManager );

	/* 複数portoを監視することも可能だが,NodeIDが変わる 初回監視ポートのみ相手に通知される -> 複数起動できるメリットはない　*/
	void start(); 
	void start( unsigned short port , int type ); // 通常とちらか一つのポート&一つのプロトコル)


	int send( KClientNode *targetNode , void* payload , unsigned short payloadLength , unsigned short protocol );
	//bool startMonitor( unsigned short port );

};



};

#endif // A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C

