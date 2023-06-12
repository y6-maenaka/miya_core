#ifndef DA884985_2465_4516_9D37_08CC9BE11556
#define DA884985_2465_4516_9D37_08CC9BE11556


#include <iostream>
#include <bitset>
#include <map>
#include <vector>
#include <cmath>

#include <stdlib.h>
#include <arpa/inet.h>

#include <thread>
#include <mutex>
#include <condition_variable>


namespace ekp2p{


constexpr unsigned short int K_BUCKET_SIZE = 160;
constexpr int NODE_ID_LENGTH = 20;
// constexpr unsigned short DEFAULT_FIND_NODE_SIZE = 5;
constexpr unsigned short K_SIZE = 8;

typedef std::bitset<160> BitSet_160; // prepare 20 bytes std::bitset
																								
																								

class Node;
class SocketManager;
struct KTag;
class KBucket;
class TableWrapper;
struct KAddr;






class KRoutingTable{

private:

	struct Wrapper
	{
		TableWrapper *_wrapper; 
		std::thread _wrapperThread;
	} _Wrapper;


	short int _maxNodeCnt = 10;
	KBucket *_bucketList[ K_BUCKET_SIZE ] = {NULL};

	
	//unsigned char* _nodeID = nullptr;
	KAddr *_kAddr;

protected:
	//void setupRoutingTable( sockaddr_in *globalAddr ); // 可変長引数でbootstrapNodeをもらう


public:
	TableWrapper *wrapper(); // getter

	KRoutingTable( unsigned short maxNodeCnt = K_SIZE );
	~KRoutingTable();
	/* ekp2p( FILE );  セーブファイルからの復帰 */ 
	// KRoutingTable( /* backup file */ ); セーブファイルから復元する場合

	bool init( sockaddr_in *globalAddr ); // tablesetup + startWrapper
	//void setupRoutingTable( unsigned char *globalAddr ); 


	bool update( Node* targetNode );
	Node* inquire( Node *targetNode ); // Nodeを与えて存在するかを確認している

	//KBucket* operator []( short int branch ) return _bucketList[ branch ];	
	KBucket* operator []( short int branch );

	void TEST_showAllBucket();

	unsigned char* NodeID(); // getter

	std::map< unsigned short, KBucket* > *ActiveKBucketList(); // getter
	
	std::vector< Node *> *nodeIterationList( unsigned short maxCnt  , Node* baseNode );  
	std::vector<Node*> *closestNodeList( unsigned short maxCnt , Node* baseNode ); // baseNodeに近い順にノードリストが作成される
};



} // close ekp2p namespace

#endif // DA884985_2465_4516_9D37_08CC9BE11556


