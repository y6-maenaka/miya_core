#include "k_routing_table.h"
#include "./k_bucket.h"

#include "./node.h"
#include "./k_tag.h"
#include "./protocol.h"

#include "../network/socket_manager/socket_manager.h"

#include "./table_wrapper/table_wrapper.h"


namespace ekp2p{

 


/* ======================================================================= */
/* ========================= [[ KRoutingTable ]] ========================= */
/* ======================================================================= */



bool KRoutingTable::init()
{
	/*
		1, ルーティングテーブルのセットアップ ファイルから復帰 of FIND_NODEによるノードの集計
		2, wrapperの起動
	 */


	setupRoutingTable();

	_Wrapper._wrapper = new TableWrapper(10000, this);	
	_Wrapper._wrapper->startThread();

}




void KRoutingTable::setupRoutingTable()
{

}







KRoutingTable::KRoutingTable( unsigned char* nodeID , short int maxNodeCnt ){

	_maxNodeCnt = maxNodeCnt;

	_myNodeID = nodeID;


	// wrapperの起動 をここで行う
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

	std::unique_lock<std::mutex> lock( _mtx ); // 以降をクリティカルセクションとする

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



Node* KRoutingTable::inquire( Node* targetNode )
{
	std::unique_lock<std::mutex> lock(_mtx);

	short int branch;
	KBucket *bucket;
	bucket = _bucketList[( branch =  targetNode->calcBranch( myNodeID() ) )]; 

	if( bucket == nullptr )
		return nullptr;

	return bucket->_nodeList->findNode( targetNode )->node();

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



