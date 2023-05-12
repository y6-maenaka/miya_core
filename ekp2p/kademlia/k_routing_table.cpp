#include "k_routing_table.h"
#include "./k_bucket.h"

#include "./node.h"
#include "./k_tag.h"
#include "./protocol.h"

#include "../network/socket_manager/socket_manager.h"


namespace ekp2p{

 


/* ======================================================================= */
/* ========================= [[ KRoutingTable ]] ========================= */
/* ======================================================================= */



KRoutingTable::KRoutingTable( unsigned char* nodeID , short int maxNodeCnt ){

	_maxNodeCnt = maxNodeCnt;

	_myNodeID = nodeID;

}



KBucket* KRoutingTable::operator []( short int branch ){ return _bucketList[ branch ]; 
}





bool KRoutingTable::autoKTagHandler( void* rawKTag , unsigned int kTagSize , SocketManager* activeSocketManager )
{

	KTag* kTag = new KTag( rawKTag, kTagSize );


	return autoKTagHandler( kTag, activeSocketManager );
}



bool KRoutingTable::autoKTagHandler( KTag* kTag, SocketManager* activeSocketManager )
{
	switch( kTag->protocol() ){

		case static_cast<int>(KADEMLIA_PROTOCOL::NORM): // case of NORM			
			update( kTag->_kAddrList[0]->toNode( activeSocketManager ) );
			break;
			
		case static_cast<int>(KADEMLIA_PROTOCOL::PING): // case of PING
			update( kTag->_kAddrList[0]->toNode( activeSocketManager) );
			kTag->_kAddrList[0]->toNode( activeSocketManager )->SendPONG();
			break;
		
		case static_cast<int>(KADEMLIA_PROTOCOL::PONG): // case of PONG
			PONGHandler( kTag->_kAddrList[0]->toNode( activeSocketManager ) ); // updateも内部で行われる
			break;

		case static_cast<int>(KADEMLIA_PROTOCOL::FIND_NODE): // case of FIND_NODE
			FIND_NODEHandler( kTag->_kAddrList[0]->toNode( activeSocketManager) , 10 );
			break;

		default: // case of erro or undefined
			break;
			// error or not defined
			
	}

	delete kTag;

	return false;
};



bool KRoutingTable::update( Node* targetNode )
{	

	// 1, branchを算出 & KBucketを特定
	// 2, bucket.findNode 

	short int branch;

	KBucket *bucket;
	bucket = _bucketList[( branch =  targetNode->calcBranch( myNodeID() ) )]; // ここは自分のekp2pに登録されているNodeIDをわたす


	// 対象のbucketがない場合は新規作成
	if( bucket == NULL )
	{
		KBucket *newBucket = new KBucket( branch );
		_bucketList[branch] = newBucket; // 新規bucketをKRoutingTableに追加
	
		newBucket->update( targetNode ); // 直接最後尾に追加してもいいが、なるべくシンプルにしたい
		return true;
	}

	bucket->update( targetNode );

	return false;
}






void KRoutingTable::TEST_showAllBucket()
{

	KBucket *bucket;
	for( int i=0; i<K_BUCKET_SIZE; i++){
		bucket = _bucketList[i];

		if( bucket == NULL )
		{
			std::cout << "[ " << i << " ]" << " Bucket is NULL";
		}
		else
		{
			std::cout << "\x1b[032m" << "[ " <<  i << " ]" << " HIT" << "\x1b[0m";	
		} 
		if( (i % 2) != 0) std::cout << "\n";
		else{ std::cout << "\t";}

	}

};



unsigned char* KRoutingTable::myNodeID(){
	return _myNodeID;
}





std::map< unsigned short, KBucket* > *KRoutingTable::ActiveKBucketList()
{																														 
	KBucket* bucket;
	std::map< unsigned short, KBucket* > *activeKBucketList = new std::map<unsigned short , KBucket*>;


	for( int i=0; i < K_BUCKET_SIZE ; i++)
	{
		bucket = (*this)[i];
		if( bucket == NULL ) continue;
		
		(*activeKBucketList)[i] = bucket;
	}	
	
	return activeKBucketList;
}



} // close ekp2p namespace 



