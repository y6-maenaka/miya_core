#ifndef DA884985_2465_4516_9D37_08CC9BE11556
#define DA884985_2465_4516_9D37_08CC9BE11556


#include <iostream>
#include <bitset>
#include <map>

#include <stdlib.h>
#include <arpa/inet.h>

#include <thread>
#include <mutex>
#include <condition_variable>


namespace ekp2p{


constexpr unsigned short int K_BUCKET_SIZE = 160;
constexpr int NODE_ID_LENGTH = 20;
constexpr unsigned short DEFAULT_FIND_NODE_SIZE = 5;

typedef std::bitset<160> BitSet_160; // prepare 20 bytes std::bitset
																								
																								

class Node;
class SocketManager;
struct KTag;
class KBucket;
class TableWrapper;










class KRoutingTable{

private:

	struct Wrapper
	{
		TableWrapper *_wrapper; 
		std::thread _wrapperThread;
	} _Wrapper;


	short int _maxNodeCnt = 10;
	KBucket *_bucketList[ K_BUCKET_SIZE ] = {NULL};

	unsigned char* _myNodeID;


protected:
	void setupRoutingTable(); // 可変長引数でbootstrapNodeをもらう


public:
	TableWrapper *wrapper(); // getter

	KRoutingTable( unsigned char* nodeID , short int maxNodeCnt );
	~KRoutingTable();
	/* ekp2p( FILE );  セーブファイルからの復帰 */ 
	// KRoutingTable( /* backup file */ ); セーブファイルから復元する場合

	bool init(); // tablesetup + startWrapper


	void NodeID( struct in_addr* ip ); // setter ( generate and set from struct of ip )
	char* NodeID(); // getter


	bool update( Node* targetNode );
	Node* inquire( Node *targetNode ); // Nodeを与えて存在するかを確認している

	//KBucket* operator []( short int branch ) return _bucketList[ branch ];	
	KBucket* operator []( short int branch );

	void TEST_showAllBucket();

	unsigned char* myNodeID(); // getter


	std::map< unsigned short, KBucket* > *ActiveKBucketList(); // getter
};



} // close ekp2p namespace

#endif // DA884985_2465_4516_9D37_08CC9BE11556


