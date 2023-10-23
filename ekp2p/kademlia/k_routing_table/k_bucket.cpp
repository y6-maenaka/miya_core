#include "k_bucket.h"

#include "../k_node.h"

namespace ekp2p
{







std::shared_ptr<KClientNode> KBucket::find( std::shared_ptr<KClientNode> target )
{
	std::unique_lock<std::mutex> lock(_mtx);

	for( KClientNodeVector::iterator i = _nodeVector.begin() ; i != _nodeVector.end() ; i++ )
		if( nodeIDcmp( target , i) == 0 ) return *i;

	return nullptr;
}



/*
int KBucket::bucketRefresh( KClientNode *target )
{
	std::vector< std::shared_ptr<KClientNode> >::iterator searchItr = _referenceHead;

	for( int i=0; i<_nodeVector.size() ; i++ )
		if( memcmp( (*searchItr)->kNodeAddr()->ID() , target->kNodeAddr()->ID() , 20 ) == 0 ) break;

	if( searchItr == _nodeVector.end() ) return -1;
	
	_nodeVector.erase( searchItr );
	_nodeVector.push_back( std::make_unique<KClientNode>(*target) );

	return 0;
}
*/


bool KBucket::move_back( std::shared_ptr<KClientNode> targetInBucket )
{
	// std::unique_lock<std::mutex> lock(_mtx); 呼び出し元でロックするから不要かも

	// 対象のイテレータを取得する
	KClientNodeVector::iterator targetInNodeIterator = std::find( _nodeVector.begin() , _nodeVector.end() , targetInBucket );
	if( targetInNodeIterator == _nodeVector.end() ) return false;

	// eraseしてpush
	std::shared_ptr<KClientNode> temp = *targetInNodeIterator;
	_nodeVector.erase( targetInNodeIterator );
	_nodeVector.push_back( temp );

	return true;
}





int KBucket::nodeIDcmp( std::shared_ptr<KClientNode> n1 , KClientNodeVector::iterator n2 )
{
	return memcmp( n1->kNodeAddr()->ID() , (*n2)->kNodeAddr()->ID() , 20 );
}




int KBucket::autoAdd( std::shared_ptr<KClientNode> target )
{
	std::unique_lock<std::mutex> lock(_mtx);
	
	std::shared_ptr<KClientNode> targetInBucket = this->find( target );

	// 以降はなからずtargetInBucketで操作する move_backでfindできなくなるから
	if( targetInBucket == nullptr ) // 対象が存在しない場合
	{
		if( isFull() )
		{
			// PING
			this->move_back( _nodeVector.at(0) ); // 一旦先頭を最後尾に移動
			_notifyNodeSwap( shared_from_this() , _nodeVector.at(0) , target );
		}
		else
		{
			_nodeVector.push_back( targetInBucket ); // 最後尾に移動
		}
	}


	else // 要素が存在する場合
	{
		this->move_back( targetInBucket );
	}


	return -1;
}


// PING機構について
// (先頭ノードにPINGを送信する) - > 帰ってきたら::そのノードをバケット最後尾に
//															- > 帰ってこなかったら::候補ノードをバケット最後尾に移動





/*
int KBucket::autoAdd( std::shared_ptr<KClientNode> target )
{
	KClientNode* existedNode;
	existedNode = this->find( target );

	// パターン一覧 
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
*/





bool KBucket::isFull()
{
	return ( _nodeVector.size() >= K_SIZE );
}



void KBucket::swapForce( std::shared_ptr<KClientNode> swapFromInBucket , std::shared_ptr<KClientNode> swapTo )
{
	std::unique_lock<std::mutex> lock(_mtx);

	KClientNodeVector::iterator targetInNodeIterator = std::find( _nodeVector.begin() , _nodeVector.end() , swapFromInBucket );
	if( targetInNodeIterator == _nodeVector.end() ) return;

	auto erasedItr = _nodeVector.erase( targetInNodeIterator );
	_nodeVector.insert(  erasedItr , swapTo	);


}


void KBucket::notifyNodeSwap( std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > target )
{
	_notifyNodeSwap = target;
}

};
