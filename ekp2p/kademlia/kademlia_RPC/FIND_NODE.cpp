#include "../table_wrapper/table_wrapper.h"

#include "./kademlia_rpc_protocol_map.h"

#include "../node.h"
#include "../k_tag.h"
#include "../k_bucket.h"
#include "../common.h"


#include "../../network/message/message.h"


#include <__config>
#include <map>
#include <vector>



namespace ekp2p{






/* FIND_NODE 手順 */






/*
void RequestFIND_NODE( unsigned char *nodeID , sockaddr_in *bootstrapNodeAddr  )
{

	Node *closestNode; // これが更新されなくなったらFIND_NODEを終了する


	// kademliaにブロードキャストを導入する
	// kademliaにclosestNodeを導入する
	UDPInbandManager *inbandManager;

	// ブートストラップノードは稼働中であることが保証されている必要がある
	// もしくは先駆けてPINGを送信するか。

	SocketManger *senderSocketManager = inbandManager->socketManager();
	unsigned char *rawFIND_NODE_MSG; unsigned int rawFIND_NODE_MSGSize;
	rawFIND_NODE_MSGSIZE = GenerateRawPRCQuery_FIND_NODE( &rawFIND_NODE_MSG );	


	for(;;)
	{
		socketManager->sendUDP( rawFIND_NODE_MSG ); 
	}
}
*/







unsigned int GenerateRawRPCQuery_FIND_NODE( unsigned char *NodeID , unsigned char **ret )
{

	EKP2PMSG rpcFIND_NODE_MSG;
	//MSGHeader *rpcFIND_NODE_MSGHeader;
	
	KTag kTag; // 自身のアドレスとノードIDを添付する
	KAddr kaddr; // 自身のアドレスを格納してkTagにadd()する
	kTag.protocol( static_cast<int>(KADEMLIA_RPC::FIND_NODE) );

	// message操作	
	rpcFIND_NODE_MSG.payload( nullptr , 0 ); // 内部でheaderも作成される	


	//rpcFIND_NODE_MSGHeader = rpcFIND_NODE_MSG->header();


	unsigned int retSize;	
	retSize = rpcFIND_NODE_MSG.exportRaw( ret );

	return retSize;
}





void RequestRPC_FIND_NODE()
{

}



bool ResponseRPC_FIND_NODE()
{
	return false;
}





/*
void TableWrapper::FIND_NODEHandler( Node* targetNode , int requestSize )
{
	 //[ 重要 ]
	  //ActiveKBucketListから基準を出すために、activeBucketListを出す前にupdateを実行する必要がある

	int offerSize;
	KBucket *bucket = NULL;

	update( targetNode );
	std::map<unsigned short , KBucket*> *activeKBucketList = ActiveKBucketList();
	std::multimap<unsigned short, KAddr*> *offerNodeMap = new std::multimap<unsigned short , KAddr*>;

	KTag *kTag = new KTag;

	int targetNodeCnt = requestSize * 3;

	// この方式だと取りすぎてしまう可能性がある まぁいい
	int i = 0;
	while( targetNodeCnt <= 0 || activeKBucketList->size() <= 0 )
	{
		for( auto itr = activeKBucketList->cbegin(); itr != activeKBucketList->cend() ; itr++ )
		{
			if(	itr->second->speNode(i) == NULL ){ activeKBucketList->erase( itr ); }
			else{	
				offerNodeMap->insert( std::make_pair( targetNode->calcBranch( itr->second->speNode(i)->nodeID() ) , itr->second->speNode(i)->kAddr() )  );
				targetNodeCnt--;
			}
		}
		i++;
	}


	auto it = offerNodeMap->cbegin();
	do
	{
		kTag->addKAddr( it->second );
		it++; requestSize--;
	}while( it != offerNodeMap->cend() || requestSize <= 0 );


	kTag->protocol( 5 );
	targetNode->send( NULL , 0 , kTag , targetNode->socketManager() );



	delete activeKBucketList;
	return;
};
*/



}

