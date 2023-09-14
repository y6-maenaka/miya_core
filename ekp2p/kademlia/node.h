#ifndef D9D8654E_C966_4A79_8059_AC11CF2976C5
#define D9D8654E_C966_4A79_8059_AC11CF2976C5



#include <iostream>
#include <bitset>


#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

// link時 依存関係エラー 本CmakeファイルにInclude Dirを追記で解決

// #include "../../ekp2p/ekp2p.h"


// #include "openssl/crypto.h"
// #include "../network/outband/node_outband.h"


namespace ekp2p{

struct KAddr;
struct KTag;
class SocketManager;
class EKP2PMSG;
class NodeOutband;
// class EKP2P;

// constexpr int NODE_ID_LENGTH = 20;
constexpr int RAW_IP_ADDRESS_LENGTH = 4;
constexpr int BIT_PER_BYTES = 8;
// constexpr int BITSET_SIZE = BIT_PER_BYTES * 20;


// typedef std::bitset<NODE_ID_LENGTH> BitSet_160; // prepare 20 bytes std::bitset


















class Node{
private:
	// unsigned char* _ip;
	//unsigned long _ip;
	KAddr* _kAddr;

	SocketManager *_leastSocketManager;

public:
	//unsigned char *_NodeID; // sha1( _ip ) // length:20[bytes]

	short int calcBranch( unsigned char* myNodeID );
	unsigned char* calcBranchRaw( unsigned char* myNodeID );

	uint32_t ip(); // getter
	void ip( uint32_t ip ); // setter
	//void ip( struct in_addr* ip );

	//BitSet_160* toBitSet_160( unsigned char* ip );

	Node();
	Node( KAddr* kAddr , SocketManager *relatedSocketManager = nullptr );
	// Node( struct in_addr *addr );
	
	~Node();

	void socketManager( SocketManager *socketManager ); // setter
	SocketManager* socketManager(); // getter

	//void NodeID( unsigned char *id ); // setter
	//unsigned char* nodeID();


	/* KADEMLIA HANDLER */
	bool SendPING(); // 後の統合する
	bool SendPONG();
	bool SendFIND_NODE();

	bool SendKademliaRPC();

	int send( EKP2PMSG *msg );
	int send( unsigned char *payload , unsigned int payloadSize , KTag *kTag );
	int send( unsigned char *msg , unsigned int msgSize );

	KAddr* kAddr(); // getter
};




} // close ekp2p namespace



#endif // D9D8654E_C966_4A79_8059_AC11CF2976C5

