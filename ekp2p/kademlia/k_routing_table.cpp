#include "k_routing_table.h"

#include "./k_bucket.h"
#include "./node.h"
#include "./k_tag.h"
#include "./protocol.h"

#include "../network/socket_manager/socket_manager.h"
#include "../network/inband/inband_manager.h"

#include "./table_wrapper/table_wrapper.h"
#include "./common.h"


namespace ekp2p{




/* ======================================================================= */
/* ========================= [[ KRoutingTable ]] ========================= */
/* ======================================================================= */


KRoutingTable::KRoutingTable( unsigned short maxNodeCnt ){

	_maxNodeCnt = maxNodeCnt;

}




bool KRoutingTable::init( sockaddr_in *globalAddr )
{
	/*
		1, ルーティングテーブルのセットアップ ファイルから復帰 of FIND_NODEによるノードの集計
		2, wrapperの起動
	 */

	// ルーティングテーブルを使用するならTableWrapperの起動は必須
	_Wrapper._wrapper = new TableWrapper(10000, this); // BufferSize + hostKRoutingTable
	_Wrapper._wrapper->start();  // wrapperの起動　 


	// NodeIDの算出
	unsigned char *nodeID = nullptr;
	calcNodeID( globalAddr , &nodeID ); // 自身のNodeIDの算出

	if( nodeID == nullptr ) {
		free( nodeID );
		return false;
	}

	//&(_kAddr->_nodeID) = nodeID; // nodeIDのセット
	_selfKAddr->nodeID( nodeID ); // nodeIDのセット

	std::cout << " === NatTraversal and Get GloablAddr Successfly Done === " << "\n";

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



KAddr* KRoutingTable::selfKAddr()
{
	return _selfKAddr;
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




unsigned char* KRoutingTable::NodeID()
{ // getter
	return _selfKAddr->_nodeID;
}





std::map< unsigned short, KBucket* > *KRoutingTable::ActiveKBucketList()
{																														 

	KBucket* bucket;
	std::map< unsigned short, KBucket* > *activeKBucketList = new std::map<unsigned short , KBucket*>;
	// branch( unsigned short ) , bucket( KBucket )	

	for( int i=0 , j =0; i < K_BUCKET_SIZE ; i++)
	{
		bucket = (*this)[i];
		if( bucket == nullptr ) continue;
		
		// (*activeKBucketList)[j] = bucket;     j++;
		
		//activeKBucketList->insert( std::make_pair(i, bucket) );
		activeKBucketList->emplace( i , bucket );
	}	

	
	return activeKBucketList;
}





std::vector<Node*> *KRoutingTable::closestNodeVector( unsigned short maxCnt , Node* baseNode )
{

	unsigned short baseBranch = baseNode->calcBranch(_selfKAddr->nodeID() ); // baseNodeのブランチを確定する
																													

	std::map< unsigned short, KBucket *> *activeKBucketList;	
	// 既存のバケットリストの取得
	if( (activeKBucketList = ActiveKBucketList()) == nullptr || activeKBucketList->size() <= 0 ) return nullptr; // 空もしくは中身がなければ即リターン

	std::vector< Node*> *closestNodeVector = new std::vector< Node*>; // closestNodeListの作成
	

	// 中心のバケットを探す	
	// ここでは簡単に,base以上のバケットを中央とする ※要変更
	std::map< unsigned short ,KBucket *>::iterator centerItr = activeKBucketList->lower_bound( baseBranch ); // キー以上の最も近い要素を獲得する
	std::map< unsigned short ,KBucket *>::iterator leftItr = centerItr;
	std::map< unsigned short ,KBucket *>::iterator rightItr = centerItr;


	std::vector< Node* > *targetKBucketNodeVector; 
	std::map< unsigned short , KBucket * >::iterator currentItr = centerItr;
	bool reachedHead = false, reachedTail = false;


	auto rippleSearch = [&]() // 波紋探索
	{
		targetKBucketNodeVector = (currentItr->second)->exportNodeVector(); // ターゲットバケット内のノードの取得
		for( auto itr = targetKBucketNodeVector->begin() ; itr != targetKBucketNodeVector->end() ; itr++ )
		{
			closestNodeVector->push_back( *itr ); // ターゲットバケット内のノードの追加
			if( closestNodeVector->size() >= maxCnt ) return;
		}


		if( reachedHead && !reachedTail )	 // 先頭に達したら
		{
			currentItr = rightItr++;
		}
		else if( !reachedHead && reachedTail )  // 最後尾に達したら
		{ 
			currentItr = leftItr++;
		}
		else if( !reachedHead && !reachedTail ) // どちらも端に達していない場合
		{
			currentItr = (pow( centerItr->first - leftItr->first , 2 ) < pow( centerItr->first - rightItr->first , 2)) ? leftItr-- : rightItr++;
		}
		else // 先頭かつ最後尾に達したら
		{
			return;
		}

		if( currentItr == activeKBucketList->begin() ) reachedHead = true;
		if( currentItr == std::prev(activeKBucketList->end()) ) reachedTail = true;
	};


	return closestNodeVector;
}




unsigned char* KRoutingTable::nodeID()
{
	return _selfKAddr->nodeID();
}





int KRoutingTable::nodeCount()
{
	std::map< unsigned short , KBucket *> *activeKBucketList;
	if( (activeKBucketList = ActiveKBucketList()) == nullptr || activeKBucketList->size() <= 0 ) return -1; // 空もしくは中身がなければ即リターン

	
	unsigned int count = 0;
	for( auto itr : *activeKBucketList )
	{
		count += itr.second->nodeList()->count();
	}

	return count;
}




std::vector<Node*> *KRoutingTable::selectNodeBatch( int maxCount, std::vector< Node*> *ignoreNodeVector )
{
				
	std::map< unsigned short , KBucket* > *activeKBucketVector = nullptr; // アクティブなKBucketのリストを用意
	activeKBucketVector = ActiveKBucketList();

	std::vector< KBucket* > shuffledKBucketVector; 
	for( auto itr : *activeKBucketVector ) shuffledKBucketVector.push_back( itr.second ); // 各バケットの取り出し
	// アクティブなKBucketのリストをシャッフルする

	unsigned int 	seed = std::chrono::system_clock::now()
														.time_since_epoch()
														.count(); // ランダムシードの作成
	shuffle( shuffledKBucketVector.begin() , shuffledKBucketVector.end(), std::default_random_engine(seed) ); // シャッフルベクとの作成

	std::map< unsigned int , KBucket* > selectiveElemPairMap; // 累積和とKBucket ペアの作成
	unsigned int currentCumulativeSum = 0; // 累積和
																				 
	for( auto itr : shuffledKBucketVector ){ // 各累積和の計算
		currentCumulativeSum += itr->nodeList()->count();
		selectiveElemPairMap[ currentCumulativeSum ] = itr;
	}

 	
	unsigned int cumulativeSum = selectiveElemPairMap.rbegin()->first; // == currentCumulativeSum
	// ランダムに取り出すためのrandomVectorを用意	
	std::vector< unsigned int > randomVector( cumulativeSum ); // ランダム取り出し用に,累積和分のランダム配列を用意する
	for( int i=0; i< cumulativeSum; i++ ){
		randomVector[i] = i; // 累積和分の配列を用意
	} shuffle( randomVector.begin(), randomVector.end(), std::default_random_engine(seed) ); // ランダムベクタの作成


	std::vector<Node*> *retNodeVector = new std::vector<Node*>; // 結果格納用ベクタ
	// 取り出しシーケンス	
	unsigned int i = 0;
	Node* candidateNode = nullptr;   KBucket *candidateKBucket = nullptr; // 一時格納用
	std::map< unsigned int , KBucket* >::iterator selectiveElemPairMapItr;
	auto itr = randomVector.cbegin();

	if( maxCount < 0 ) maxCount = nodeCount();
	while( retNodeVector->size() <= maxCount || itr != randomVector.end() ) // 指定の個数に到達するか,テーブルにそれ以上ノードがなくなったら終了
	{
		selectiveElemPairMapItr = selectiveElemPairMap.lower_bound( *itr ); // 一旦イテレータで受け取る
		candidateKBucket = selectiveElemPairMapItr->second;
		candidateNode = (*candidateKBucket)[ selectiveElemPairMapItr->first - *itr ]; // 累積和 - ランダム値

		// ignoreNodeだったら無視する
		if( std::count( ignoreNodeVector->cbegin(), ignoreNodeVector->cend(), candidateNode ) != 0 ){
			itr++; continue;
		}

		retNodeVector->push_back( candidateNode );

		itr++;
	}
	
	return retNodeVector;		
}













}; // close ekp2p namespace 





