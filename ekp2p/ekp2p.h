#ifndef A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C
#define A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C


#include <iostream>
#include <arpa/inet.h>
// #include <sys/_types/_int64_t.h>
#include <sys/socket.h>

#include <memory>

// #include "./allowed_protocol_set.h"



constexpr unsigned short DEFAULT_BIND_PORT = 8080;
constexpr int DEFAULT_FIND_NODE_ROUTINE_TIMEOUT = 10;



struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;






namespace ekp2p{




class InbandNetworkManager;
class KRoutingTable;
class SocketManager;
class SmartMiddleBuffer;

class KNodeAddr;
class KHostNode;
class KClientNode;



class Sender;
class Receiver;
class KRoutingTableUpdator;



constexpr unsigned short _PROTOCOL_NAT_ = 3;





class EKP2P // 基本的にNAT超え後はそのSocketManagerを使い回し続ける必要がありそう
{

private:

	//std::shared_ptr<KHostNode> _hostNode;  ホストノードを持つのはkademliaレイヤーだけでいい?
	std::shared_ptr<KRoutingTable> _kRoutingTable;
	std::shared_ptr<SocketManager> _hostSocketManager;

	struct 
	{
		std::shared_ptr<Sender>	_sender;
		std::shared_ptr<StreamBufferContainer> _toSenderSB;
	} _senderDaemon;


	struct
	{
		std::shared_ptr<Receiver> _receiver;
		std::shared_ptr<StreamBufferContainer> _toReseiverSB;
	} _receiverDaemon;

	struct
	{
		std::shared_ptr<KRoutingTableUpdator> _updator;
		std::shared_ptr<StreamBufferContainer> _toUpdatorSB;
	} _updatorDaemon;



public:
	//EKP2P( KRoutingTable *baseKRoutingTable = nullptr );
	EKP2P( std::shared_ptr<KRoutingTable> kRoutingTable = nullptr /* バックアップから復旧する場合*/ );
	 
	bool collectStartUpNodes( SocketManager *baseSocketManager );

	/* 複数portoを監視することも可能だが,NodeIDが変わる 初回監視ポートのみ相手に通知される -> 複数起動できるメリットはない　*/
	int init(); // KRoutingTableを使うのであれば必須 自身のグローバルアドレスを取得する
	int start();


	int send( KClientNode *targetNode , void* payload , unsigned short payloadLength , unsigned short protocol );
	//bool startMonitor( unsigned short port );


	std::shared_ptr<StreamBufferContainer> toReseiverSB();
	std::shared_ptr<StreamBufferContainer> toSenderSB();
	std::shared_ptr<StreamBufferContainer> toUpdatorSB();
};



};

#endif // A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C

