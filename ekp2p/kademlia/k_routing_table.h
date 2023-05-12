#ifndef DA884985_2465_4516_9D37_08CC9BE11556
#define DA884985_2465_4516_9D37_08CC9BE11556



#include <iostream>
#include <bitset>
#include <map>

#include <stdlib.h>
#include <arpa/inet.h>


namespace ekp2p{


constexpr unsigned short int K_BUCKET_SIZE = 160;
constexpr int NODE_ID_LENGTH = 20;
constexpr unsigned short DEFAULT_FIND_NODE_SIZE = 5;

typedef std::bitset<160> BitSet_160; // prepare 20 bytes std::bitset
																								
																								

class Node;
class SocketManager;
struct KTag;
class KBucket;











class KRoutingTable{

private:
	short int _maxNodeCnt = 10;
	KBucket *_bucketList[ K_BUCKET_SIZE ] = {NULL};

	unsigned char* _myNodeID;

public:
	void NodeID( struct in_addr* ip ); // setter ( generate and set from struct of ip )
	char* NodeID(); // getter

	KRoutingTable( unsigned char* nodeID , short int maxNodeCnt );
	/* ekp2p( FILE );  セーブファイルからの復帰 */ 
	// KRoutingTable( /* backup file */ ); セーブファイルから復元する場合

	// インターフェース周り
	bool autoKTagHandler( void* rawKTag , unsigned int kTagSize , SocketManager* activeSocketManager = NULL );
	bool autoKTagHandler( KTag* kTag, SocketManager* activeSocketManager = NULL );

	bool update( Node* targetNode );

	//KBucket* operator []( short int branch ) return _bucketList[ branch ];	
	KBucket* operator []( short int branch );

	void TEST_showAllBucket();

	unsigned char* myNodeID(); // getter

	void setupKRoutingTable(); // 可変長引数でbootstrapNodeをもらう

	void PINGHandler( Node *targetNode );
	void PONGHandler( Node *targetNode );
	void FIND_NODEHandler( Node* targetNode  , int requestSize );

	std::map< unsigned short, KBucket* > *ActiveKBucketList(); // getter
};



} // close ekp2p namespace

#endif // DA884985_2465_4516_9D37_08CC9BE11556


