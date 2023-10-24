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

struct KNodeAddr;
class KHostNode;
class KClientNode;

struct EKP2PMessage;
struct EKP2PMessageHeader;


class EKP2PBroker;
class EKP2PSender;
class EKP2PReceiver;
class EKP2PKRoutingTableUpdator;



constexpr unsigned short _PROTOCOL_NAT_ = 3;

constexpr unsigned short DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER = 0;
constexpr unsigned short DEFAULT_DAEMON_FORWARDING_SBC_ID_NATER = 1; // Nat超え用モジュールも一応デフォルトモジュールとする




class EKP2P // 基本的にNAT超え後はそのSocketManagerを使い回し続ける必要がありそう
{

private:

	//std::shared_ptr<KHostNode> _hostNode;  ホストノードを持つのはkademliaレイヤーだけでいい?
	std::shared_ptr<KRoutingTable> _kRoutingTable;
	std::shared_ptr<SocketManager> _hostSocketManager;

	struct 
	{
		std::shared_ptr<EKP2PBroker> _broker;
		std::shared_ptr<StreamBufferContainer> _toBrokerSBC;
	} _brokerDaemon;

	struct 
	{
		std::shared_ptr<EKP2PSender>	_sender;
		std::shared_ptr<StreamBufferContainer> _toSenderSBC;
	} _senderDaemon;


	struct
	{
		std::shared_ptr<EKP2PReceiver> _receiver;
		std::shared_ptr<StreamBufferContainer> _toReseiverSBC;
	} _receiverDaemon;

	struct
	{
		std::shared_ptr<EKP2PKRoutingTableUpdator> _manager;
		std::shared_ptr<StreamBufferContainer> _toManagerSBC;
	} _routingTableManagerDaemon;



public:
	//EKP2P( KRoutingTable *baseKRoutingTable = nullptr );
	EKP2P( std::shared_ptr<KRoutingTable> kRoutingTable = nullptr /* バックアップから復旧する場合*/ );
	 
	bool collectStartUpNodes( SocketManager *baseSocketManager );

	/* 複数portoを監視することも可能だが,NodeIDが変わる 初回監視ポートのみ相手に通知される -> 複数起動できるメリットはない　*/
	int init( std::string stunServerAdddrListPath ); // KRoutingTableを使うのであれば必須 自身のグローバルアドレスを取得する
	int initCustom();
	int start( bool requiresRouting = true );


	int send( KClientNode *targetNode , void* payload , unsigned short payloadLength , unsigned short protocol );
	//bool startMonitor( unsigned short port );


	std::shared_ptr<StreamBufferContainer> toBrokerSBC();
	std::shared_ptr<StreamBufferContainer> toReseiverSBC();
	std::shared_ptr<StreamBufferContainer> toSenderSBC();
	std::shared_ptr<StreamBufferContainer> toRoutingTableManagerSBC();

	int assignBrokerDestination( std::shared_ptr<StreamBufferContainer> forwardingSBC , unsigned short destination );


	static void sendDimmyEKP2PMSG( const char* destIP, unsigned short destPort , std::shared_ptr<unsigned char> content , size_t contentLength , struct sockaddr_in sourceAddr  , unsigned short rpcType = 0 , unsigned short protocol = 0 );
	static std::shared_ptr<EKP2PMessage> receiveSingleEKP2PMSG( unsigned short listenPort );
};



};

#endif // A31DECDF_1BF2_44C4_B9C7_4B5BB04F617C

