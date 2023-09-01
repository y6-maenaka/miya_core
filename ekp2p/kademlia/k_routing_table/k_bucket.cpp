#include "k_bucket.h"

#include "../k_node.h"

namespace ekp2p
{








KClientNode* KBucket::find( KNodeAddr* target )
{

	std::vector< std::shared_ptr<KClientNode> >::iterator searchItr = _referenceHead;

	for( int i=0; i<_kBucket.size() ; i++ )
		if( memcmp( (*searchItr)->kNodeAddr()->ID() , target->ID() , 20 ) == 0 ) return (*searchItr).get(); 

	return nullptr;
}





int KBucket::add( KClientNode* target )
{
	if( _kBucket.size() >= K_SIZE ) return -1; // ノード入れ替えを実行する

	_kBucket.push_back( std::make_shared<KClientNode>(*target) ); // 単純追加　
	return 0;
}






int KBucket::bucketRefresh( KClientNode *target )
{
	std::vector< std::shared_ptr<KClientNode> >::iterator searchItr = _referenceHead;

	for( int i=0; i<_kBucket.size() ; i++ )
		if( memcmp( (*searchItr)->kNodeAddr()->ID() , target->kNodeAddr()->ID() , 20 ) == 0 ) break;

	if( searchItr == _kBucket.end() ) return -1;
	
	_kBucket.erase( searchItr );
	_kBucket.push_back( std::make_unique<KClientNode>(*target) );

	return 0;
}




int KBucket::autoAdd( KNodeAddr* target )
{
	KClientNode* existedNode;
	existedNode = this->find( target );

	/* パターン一覧 */
	// 1. 対象のノードが存在して,バケットが満杯の場合        -> 最後尾に移動する
	// 2. 対象のノードが存在して,バケットが満杯でない場合    -> 最後尾に移動する
	// 3. 対象のノードが素材せず,バケットが満杯の場合        -> スワップルーチンを起動する
	// 4. 対象のノードが存在せず,バケットが満杯でない場合    -> 単純に追加する
	
	if( existedNode != nullptr && isFull() )
	{
		this->bucketRefresh( existedNode );
	}
	else if( existedNode != nullptr && !(isFull()) )
	{
		this->bucketRefresh( existedNode );
	}
	else if( existedNode == nullptr && isFull() )
	{
		// スワップルーチンを起動する
	}else
	{
		this->add( existedNode );
	}

	return 0;
}






bool KBucket::isFull()
{
	return ( _kBucket.size() >= K_SIZE );
}




};
