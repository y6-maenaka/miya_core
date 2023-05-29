#include "k_routing_table.h"
#include "./k_bucket.h"

#include "./node.h"
#include "./k_tag.h"
#include "./protocol.h"

#include "../network/socket_manager/socket_manager.h"

#include "./table_wrapper/table_wrapper.h"
#include "./common.h"


namespace ekp2p{

 


/* ======================================================================= */
/* ========================= [[ KRoutingTable ]] ========================= */
/* ======================================================================= */




KRoutingTable::KRoutingTable( short int maxNodeCnt ){

	_maxNodeCnt = maxNodeCnt;

}




bool KRoutingTable::init( sockaddr_in *globalAddr )
{
	/*
		1, ルーティングテーブルのセットアップ ファイルから復帰 of FIND_NODEによるノードの集計
		2, wrapperの起動
	 */

	_Wrapper._wrapper = new TableWrapper(10000, this); // BufferSize + hostKRoutingTable
	_Wrapper._wrapper->startThread();  // wrapperの起動　 

	// NodeIDの算出
	unsigned char *nodeID = nullptr;
	calcNodeID( globalAddr, &nodeID );

	if( nodeID == nullptr ) {
		free( nodeID );
		return false;
	}

	_nodeID = nodeID;

	return true;
}



/*
void KRoutingTable::setupRoutingTable( sockaddr_in *globalAddr ) // Nat超えしてグローバルIPを取得する
{
	return;
}
*/







TableWrapper *KRoutingTable::wrapper(){
	return _Wrapper._wrapper;
}





KRoutingTable::~KRoutingTable()
{
	delete _Wrapper._wrapper;
}




KBucket* KRoutingTable::operator []( short int branch ){ return _bucketList[ branch ]; 
}



bool KRoutingTable::update( Node* targetNode ) // 一部をクリティカルセクションとする
{	

	// 1, branchを算出 & KBucketを特定
	// 2, bucket.findNode 

	short int branch;

	KBucket *bucket;
	bucket = _bucketList[( branch =  targetNode->calcBranch( NodeID() ) )]; // ここは自分のekp2pに登録されているNodeIDをわたす


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



Node* KRoutingTable::inquire( Node* targetNode )
{

	short int branch;
	KBucket *bucket;
	bucket = _bucketList[( branch =  targetNode->calcBranch( NodeID() ) )]; 

	if( bucket == nullptr )
		return nullptr;

	return bucket->find( targetNode )->node();

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




unsigned char* KRoutingTable::NodeID(){ // getter
	return _nodeID;
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


}; // close ekp2p namespace 



