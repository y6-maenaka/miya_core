#ifndef FFAD00AB_D1C6_4CCC_905D_3937E4270797
#define FFAD00AB_D1C6_4CCC_905D_3937E4270797


#include "../protocol.h"

#include <iostream>
#include <thread>

class MiddleBuffer;
class KRoutingTable;
class SocketManager;
class Node;
struct KTag;


namespace ekp2p{






class TableWrapper
{

private:
	
	MiddleBuffer *_middleBuffer;
	KRoutingTable *_hostTable; // 更新対象のKademliaテーブル

public:
	TableWrapper(  unsigned int bufferSize , KRoutingTable *_hostTable );
	~TableWrapper();

	// インターフェース周り
	bool autoKTagHandler( void* rawKTag , unsigned int kTagSize , SocketManager* activeSocketManager = nullptr );
	bool autoKTagHandler( KTag* kTag, SocketManager* activeSocketManager = nullptr );

	// KademliaProtocol関係のハンドラ
	void PINGHandler( Node *targetNode );
	void PONGHandler( Node *targetNode );
	void FIND_NODEHandler( Node* targetNode  , int requestSize );

	void startThread();
	void joinThread();

	MiddleBuffer *middleBuffer();


};


}; // close ekp2p namespace




#endif // FFAD00AB_D1C6_4CCC_905D_3937E4270797
