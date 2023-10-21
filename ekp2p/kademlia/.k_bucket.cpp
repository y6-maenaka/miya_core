#include "k_bucket.h"

#include "./node.h"
#include "./k_tag.h"
#include "./protocol.h"

#include "../network/socket_manager/socket_manager.h"



namespace ekp2p{

void* ReplacementThread( void* args );


NodeListElem::NodeListElem( Node* targetNode ) : _prevElemPtr(NULL), _nextElemPtr(NULL)
{
	_body._node = targetNode;
}








void NodeListElem::isReserved( bool isReserved )
{
	_body._isReserved = isReserved;
}



bool NodeListElem::isReserved()
{
	return _body._isReserved;
}
	



Node* NodeListElem::node()
{
	return _body._node;
};



void NodeListElem::node( Node* node )
{
	_body._node = node;
};




NodeListElem *NodeListElem::next(){
	return _nextElemPtr;
}


void NodeListElem::next( NodeListElem* nextElemPtr ){
	_nextElemPtr = nextElemPtr;
}


																
NodeListElem *NodeListElem::prev(){
	return _prevElemPtr;
}



void NodeListElem::prev( NodeListElem* prevElemPtr ){	
	_prevElemPtr = prevElemPtr;
}



void NodeListElem::swapNode( Node* targetNode )
{
	_body._node = 	targetNode;
}




NodeListElem::Body* NodeListElem::body(){
	return &_body;
}
								
								
								
/* ======================================================================= */
/* =========================== [[ NodeList ]] ============================= */
/* ======================================================================= */




NodeList::NodeList( int maxNodeCnt ) :  _front(NULL), _nodeCnt(0)
{
	_listSize = maxNodeCnt;
};




// [ exist : return -1 ] , [ not exist : return idx ]
/*
NodeListElem *NodeList::findNode( Node* targetNode )
{
				
	NodeListElem *ch;
	ch = _front;

	if( ch == NULL ) return NULL;

	do
	{
		if( ch->node() == targetNode ) return ch;
		ch = ch->next();

	}while( ch != _front );

	return NULL;
};
*/




NodeListElem *NodeList::front(){  // getter
	return _front;
};




bool NodeList::push_back( Node *targetNode ){

	NodeListElem *newElem = new NodeListElem( targetNode );

	return push_back( newElem );
	
};



bool NodeList::push_back( NodeListElem *targetElem )
{

	std::cout << "PUSH BACK NOW" << "\n";


	// 収容範囲を超えていないかチェクする
	if( size() <= count() ) return false;


	if( _front == NULL ){ // 空のリストへの追加
		_front = targetElem;
		targetElem->prev( targetElem ) ; targetElem->next( targetElem );

		_nodeCnt += 1;
		return true;
	}
	
	// そうさ
	_front->prev()->next( targetElem );
	targetElem->prev( _front->prev() );
	targetElem->next( _front );
		
	_front->prev( targetElem );

	/*
	if( _front->next() == _front )
		_front->next( targetElem );
		*/

	_nodeCnt += 1;

	return true;
}


void NodeList::push_front( Node *targetNode ){

	// routingでは 使わない	
	
};


void NodeList::toRear( NodeListElem* targetElem ){

	// 移動対象が先頭にあった場合は先頭ポインタをずらすだけ
	if( targetElem == _front )
	{ //　登録Elem数に依らない
		_front = _front->next(); // これで対象Elemが最後尾に移動する
		return;
	}	 

	targetElem->prev()->next( targetElem->next() );
	targetElem->next()->prev( targetElem->prev() );

	/* push_backまでの動作を完全に保証する必要がある */	
	_nodeCnt -= 1; // あまりよくない気がする
	push_back( targetElem );

	return;
};



unsigned short NodeList::size()
{
	return _listSize;
};



unsigned short NodeList::count()
{
	return _nodeCnt;
};


std::vector<Node *> *NodeList::exportNodeVector()
{
	if( _front == NULL ) return NULL;

	NodeListElem *pos = _front;
	std::vector<Node*> *nodeList = new std::vector<Node*>;	

	do{
		nodeList->push_back( pos->node() );
		pos = pos->next();

	}while( pos != _front );

	return nodeList;
}


void NodeList::show_Test(){

	std::cout << "======================" << "\n";

	std::cout <<  "size : " << size() << "\n";
	std::cout <<  "count : " << count() << "\n";
	printf("front : %p \n", _front );

	NodeListElem *pos = NULL;
	pos = _front;
	int i=0;

	std::cout << "------------------------------" << "\n";
	do
	{
		printf("[ %d ] - %p\n", i, pos );
		pos = pos->next();  i++;

	}while( pos != _front );
	std::cout << "------------------------------" << "\n";


	std::cout << "======================" << "\n";
};

/* ======================================================================= */
/* =========================== [[ KBUcket ]] ============================= */
/* ======================================================================= */




KBucket::KBucket( unsigned short branch )
{	
	_branch = branch;
	_nodeList = new NodeList( 3 );
																		
}


NodeList* KBucket::nodeList()
{
	return _nodeList;
}
							





unsigned short KBucket::branch(){ // getter
	return _branch;
}





NodeListElem* KBucket::update( Node *targetNode ){

	std::unique_lock<std::mutex> lock(_mtx); // unique_lockはスコープを抜けるとmutexを勝手に解放してくれる

	NodeListElem *targetElem = NULL;

	targetElem = find( targetNode );

	/*
	printf("%p\n", targetElem );
	std::cout << _nodeList->size() << "\n";
	std::cout << _nodeList->count() << "\n";
	*/

  /* 既に対象のノードが登録されておりBucketに空きがある場合  */	
	if(  targetElem != NULL &&_nodeList->count() < _nodeList->size() )
	{
		// 対象のエレメントを最後尾に移動させる
		_nodeList->toRear( targetElem );
		std::cout << "[ UPDATE STATU ] " << "既に対象のノードが登録されておりBucketに空きがある" << "\n";
		return NULL;
	}


	/* 既に対象のノードが登録されておりBucketが満杯の時 */
	else if( targetElem != NULL && _nodeList->count() == _nodeList->size() )
	{
		_nodeList->toRear( targetElem );
		std::cout << "[ UPDATE STATU ] " << "既に対象のノードが登録されておりBucketが満杯" << "\n";
		return NULL;
	}

	/* ノードが登録されておらずBucketが満杯 */
	else if( targetElem == NULL && _nodeList->count() == _nodeList->size() )
	{

		/*
		 1. 先頭ノードを取得
		 2. 先頭ノードにPINGを送信
		 3. 先頭ノードを最後尾に移動
		 4. 予約フラグを立てる
		 5. 置換スレッドを起動
		*/

		NodeListElem *frontElem = _nodeList->front(); // 1

		frontElem->node()->SendPING();  // 2
		_nodeList->toRear( frontElem ); // 3
		frontElem->isReserved( true ); // 4
	
		StartReplacementThread( frontElem , targetNode,  10 ); // 5 // arg:3 -> wait_time 

		std::cout << "[ UPDATE STATU ] " << "ノードが登録されおらずBucketが満杯" << "\n";
		return NULL;
		// return Elem
	}


	/* ノードが登録されておらずBucketに空きがある */
	_nodeList->push_back( targetNode ); // 内部でRingBuffElemは生成
	std::cout << "[ UPDATE STATU ] " << "ノードが登録されておらずBucketに空きがある" << "\n";

	return nullptr;	
}



NodeListElem* KBucket::find( Node *targetNode )
{
	std::unique_lock<std::mutex> lock(_mtx);

	NodeListElem *ch;
	ch = _nodeList->_front;

	if( ch == NULL ) return NULL;

	do
	{
		if( ch->node() == targetNode ) return ch;
		ch = ch->next();

	}while( ch != _nodeList->_front );

	return NULL;

}




Node* KBucket::operator[]( int index ){

	NodeListElem *retElem = NULL;
	
	if( retElem == NULL ) return NULL;

	retElem = _nodeList->front()->prev();

	while( retElem != _nodeList->front() || index >= 0 ){
		retElem = retElem->prev();
		index--;
	}

	if( index <= 0 ) return retElem->node();

	return NULL;

}




Node* KBucket::speNode( int index )
{
	return (*this)[index];
}



void KBucket::StartReplacementThread( NodeListElem *targetElem ,  Node *candidateNode , unsigned int waitTime )
{
	pthread_t threadID;
	ReplacementThreadArgs *args = new ReplacementThreadArgs( targetElem , candidateNode , waitTime );

	int ret;
	ret = pthread_create( &threadID , NULL , ReplacementThread , (void *)args  );

	if( ret == 0 )
		std::cout << "success -> start thread" << "\n";
}



void* ReplacementThread( void* args )
{
	ReplacementThreadArgs *_args;
	_args = (ReplacementThreadArgs *)(args);

	sleep( _args->waitTime );

	// waitTIme 秒待ってもPONGが帰ってこなかったら離脱したと判断して候補ノードと入れ替える
	if( _args->targetElem->isReserved() ){
		_args->targetElem->swapNode( _args->candidateNode );
		_args->targetElem->isReserved( false );
	}

	delete _args;
	return NULL;
}




std::vector< Node* > *KBucket::exportNodeVector()
{
	return _nodeList->exportNodeVector();
}






}; // close ekp2p namespace
