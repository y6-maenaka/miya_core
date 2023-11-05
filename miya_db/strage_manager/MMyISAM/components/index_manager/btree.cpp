#include "btree.h"
#include "../page_table/optr_utils.h"

#include "../page_table/overlay_memory_manager.h"

namespace miya_db
{




void ViewItemSet::importItemSet( std::shared_ptr<ONodeItemSet> itemSet )
{
	std::array< std::shared_ptr<unsigned char>, DEFAULT_KEY_COUNT> *keySorce = itemSet->exportKeyArray(); // キーを一旦配列として書き出す
	std::copy( keySorce->begin() ,keySorce->end() , _key.begin() ); // 上書きで大丈夫

	std::array< std::shared_ptr<optr> , DEFAULT_DATA_OPTR_COUNT > *dataOptrSource = itemSet->exportDataOptrArray();
	std::copy( dataOptrSource->begin() , dataOptrSource->end(), _dataOPtr.begin() );

	std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT> *childSorce = itemSet->exportChildArray(); // 子ノードを一旦配列として書き出す
	std::copy( childSorce->begin() ,childSorce->end(), _child.begin() ); // 上書き

	return;
}




void ViewItemSet::moveInsertChild( unsigned short index ,std::shared_ptr<ONode> target )
{
	for( int i = _child.size()-1; i > index; i-- )
		_child.at(i) = _child.at(i-1);

	_child.at(index) = target;
}


void ViewItemSet::moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> target )
{
	for( int i = _dataOPtr.size()-1; i > index; i-- )
		_dataOPtr.at(i) = _dataOPtr.at(i-1);
	
	_dataOPtr.at(index) = target;
}



/*
	-------------------- 初期化関係 -----------------------
*/



ONodeItemSet::ONodeItemSet( std::shared_ptr<optr> __optr )
{
	_optr = nullptr;
	_optr = __optr;
}



std::shared_ptr<optr> ONodeItemSet::Optr()
{
	return _optr;
}

void ONodeItemSet::clear()
{
	unsigned char zeroBuff[O_NODE_ITEMSET_SIZE]; memset( zeroBuff , 0x00 , sizeof(zeroBuff) );
	omemcpy( _optr.get(), zeroBuff, sizeof(zeroBuff) );
}


std::shared_ptr<optr> ONodeItemSet::parent() // 無限ループする可能性あり
{
	unsigned char oAddr[5];
	omemcpy( oAddr , _optr , 5 );

	return std::make_shared<optr>( oAddr , _optr->cacheTable() );
	//return _optr;
}


/*
  ---------------- キー操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::keyCount()
{
	return static_cast<unsigned short>( (*_optr + KEY_ELEMENT_OFFSET)->value() ); // itemSet先頭1バイトがそのままkeyCountになる
}

std::shared_ptr<unsigned char> ONodeItemSet::rawKey( unsigned short index )
{
	std::shared_ptr<optr> key = this->key( index );
	if( key == nullptr ) return nullptr;

	std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
	omemcpy( ret.get() , key.get(), KEY_SIZE );
	return ret;
}

std::shared_ptr<optr> ONodeItemSet::key( unsigned short index ) // index : start with 0
{
	if( index > (keyCount()) ) return nullptr;

	return *_optr + ( KEY_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index * KEY_SIZE));
}

void ONodeItemSet::key( unsigned short index , std::shared_ptr<unsigned char> targetKey ) // キーの挿入
{
	if( index > DEFAULT_KEY_COUNT ) return;

	omemcpy( (*_optr + ( KEY_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index * KEY_SIZE))).get() , targetKey.get() , KEY_SIZE );
}


void ONodeItemSet::keyCount( unsigned short num ) // キー個数の挿入
{
	(*_optr + KEY_ELEMENT_OFFSET)->value( static_cast<unsigned char>(num) );
}




void ONodeItemSet::sortKey()
{
	// loke buble sort
	std::shared_ptr<optr> oKey_j0; std::shared_ptr<optr> oKey_j1;
	std::shared_ptr<unsigned char> key_j0 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	std::shared_ptr<unsigned char> key_j1 = std::shared_ptr<unsigned char>( new unsigned char[20] );


	for( int i=0; i < keyCount()-1; i++ )
	{
		for( int j=keyCount()-1; j>i; j-- )
		{
			oKey_j0 = key(j);  oKey_j1 = key(j-1);
			omemcpy( key_j0.get(), oKey_j0, KEY_SIZE ); omemcpy( key_j1.get(), oKey_j1, KEY_SIZE );
			if( memcmp( key_j1.get() , key_j0.get() , KEY_SIZE ) > 0  )
			{
				omemcpy( oKey_j0.get() , key_j1.get() , KEY_SIZE );
				omemcpy( oKey_j1.get() , key_j0.get() , KEY_SIZE );
			}
		}
	}
}

void ONodeItemSet::moveInsertKey( unsigned short index , std::shared_ptr<unsigned char> targetKey )
{
	for( int i = keyCount(); i > index; i-- )
		key( i , rawKey(i-1) );

	key( index , targetKey  );
}



void ONodeItemSet::moveDeleteKey( unsigned short index )
{
	if( keyCount() <= 0 ) return;

	//std::shared_ptr<optr> keyOptr;
	for( int i=index; i<keyCount() - 1; i++ )
	{
		//std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		//keyOptr = key(i+1); omemcpy( rawKey.get() , keyOptr , KEY_SIZE );
		key( i ,  rawKey(i+1) );
	}

	keyCount( keyCount() - 1 );
}
/* -------------------------------------------------------------- */




/*
	---------------- 子ノード操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::childCount()
{
	return static_cast<unsigned short>( (*_optr +( CHILD_ELEMENT_OFFSET ))->value() );
}

void ONodeItemSet::childCount( unsigned short num )
{	
	(*_optr + ( CHILD_ELEMENT_OFFSET ))->value( static_cast<unsigned char>(num)) ;
}


std::shared_ptr<ONode> ONodeItemSet::child( unsigned short index )
{
	// 指定インデックスのoptrを取得する
	if( index >= (childCount()) ) return nullptr;
			
	std::shared_ptr<optr> childHead = *_optr + ( CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE) );
	
	unsigned char oAddr[5];
	omemcpy( oAddr , childHead , 5 );

	std::shared_ptr<optr> childOptr = std::make_shared<optr>( oAddr, childHead->cacheTable() );
	
	return std::make_shared<ONode>( childOptr);
}






void ONodeItemSet::child( unsigned short index , std::shared_ptr<ONode> targetONode )
{
	if( index > DEFAULT_CHILD_COUNT ) return;
	if( targetONode == nullptr )
	{
		unsigned char addrZero[5]; memset( addrZero, 0x00 , sizeof(addrZero) );
		omemcpy( (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , addrZero , NODE_OPTR_SIZE );
		return;
	}


	omemcpy( (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , targetONode->itemSet()->Optr()->addr() , NODE_OPTR_SIZE );
}


void ONodeItemSet::moveInsertChild( unsigned short index , std::shared_ptr<ONode> targetONode )
{
	for( int i = childCount(); i > index; i-- )
		child(i, child(i-1) );

	child(index, targetONode );
}


void ONodeItemSet::moveDeleteChild( unsigned short index )
{
	if( childCount() <= 0 ) return;

	for( int i = index; i<childCount()-1; i++ )
	{
		std::cout << "do moveDelete child" << "\n";
		child( i , child(i+1) );
	}
	childCount( childCount() - 1 );
}


/* -------------------------------------------------------------- */




/*
	---------------- データオーバレイポインタ操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::dataOptrCount()
{
	return static_cast<unsigned short>( (*_optr + DATA_OPTR_ELEMENT_OFFSET )->value() );
}

void ONodeItemSet::dataOptrCount( unsigned short num )
{
	(*_optr + ( DATA_OPTR_ELEMENT_OFFSET ))->value( static_cast<unsigned char>(num)) ;
}

std::shared_ptr<optr> ONodeItemSet::dataOptr( unsigned short index )
{
	if( index > (dataOptrCount()) ) return nullptr;

	std::shared_ptr<unsigned char> retOAddr = std::shared_ptr<unsigned char>( new unsigned char[DATA_OPTR_SIZE] );
	omemcpy( retOAddr.get(), *_optr + (( DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE) + (index*DATA_OPTR_SIZE)) , DATA_OPTR_SIZE );

	std::shared_ptr<optr> ret = std::make_shared<optr>( retOAddr.get() ); // そのままリターンでいい

	return ret;
}


void ONodeItemSet::dataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr )
{
	if( targetDataOptr == nullptr )
	{
		unsigned char addrZero[5]; memset( addrZero, 0x00 , sizeof(addrZero) );
		omemcpy( (*_optr + (DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , addrZero , NODE_OPTR_SIZE );
		return;
	}

	omemcpy( (*_optr + (DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , targetDataOptr->addr() , NODE_OPTR_SIZE );
}



void ONodeItemSet::moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr )
{
	for( int i = dataOptrCount(); i > index; i-- )
		dataOptr( i , dataOptr(i-1) );

	dataOptr( index ,  targetDataOptr );
}

void ONodeItemSet::moveDeleteDataOptr( unsigned short index )
{
	if( dataOptrCount() <= 0 ) return;

	for( int i=index; i<dataOptrCount()-1; i++)
		dataOptr( i , dataOptr(i+1) );

	dataOptrCount( dataOptrCount() - 1 );
}



void ONodeItemSet::remove( unsigned short index )
{
	moveDeleteKey( index );
	moveDeleteDataOptr( index );
}
/* -------------------------------------------------------------- */




std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *ONodeItemSet::exportKeyArray()
{
	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *ret = new std::array< std::shared_ptr<unsigned char>  , DEFAULT_KEY_COUNT>;

	for( int i=0; i<keyCount(); i++)
		ret->at(i) = key(i)->mapToMemory( KEY_SIZE );	

	return ret;
}



std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT> *ONodeItemSet::exportChildArray()
{
	std::array< std::shared_ptr<ONode>, DEFAULT_CHILD_COUNT> *ret = new std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT>;

	for( int i=0; i < childCount(); i++ )
		ret->at(i) = child(i);
	
	return ret;
}



std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *ONodeItemSet::exportDataOptrArray()
{
	std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *ret = new std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT >;

	for( int i=0; i< dataOptrCount(); i++ )
		ret->at(i) = dataOptr(i);

	return ret;
}


















ONode::ONode( std::shared_ptr<optr> baseOptr ) // これではメモリマネージャーがセットされないので必ずONodeを介すこと
{	
	_itemSet = std::make_shared<ONodeItemSet>( baseOptr );
}



ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;

	std::shared_ptr<optr> baseOptr = oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // 新規作成の場合
	_itemSet = std::make_shared<ONodeItemSet>( baseOptr );
	_itemSet->clear(); // ゼロ埋めする
}


ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr )
{
	_oMemoryManager = oMemoryManager;
	_itemSet = std::make_shared<ONodeItemSet>( baseOptr );
}



std::shared_ptr<ONode> ONode::parent()
{
	//return std::make_shared<ONode>( new ONode( _itemSet->Optr()) );
	return std::make_shared<ONode>( _oMemoryManager , _itemSet->parent() );
}




void ONode::parent( std::shared_ptr<ONode> target )
{
	if( target == nullptr ) // 空の場合はnull(0x00)を挿入する
	{
		unsigned char *tmp = new unsigned char[NODE_OPTR_SIZE]; memset( tmp , 0x0000000000, sizeof(NODE_OPTR_SIZE) );
		omemcpy( _itemSet->Optr() , tmp , NODE_OPTR_SIZE );
		return;
	}


	omemcpy( _itemSet->Optr() , target->itemSet()->Optr()->addr() , NODE_OPTR_SIZE );

	return;
}



std::shared_ptr<ONodeItemSet> ONode::itemSet()
{
	return _itemSet;
}





std::shared_ptr<ONode> ONode::recursiveAdd( std::shared_ptr<unsigned char> targetKey, std::shared_ptr<optr> targetDataOptr, std::shared_ptr<ONode> targetONode )
{
	if( _itemSet->keyCount() >= DEFAULT_KEY_COUNT ) // ノードの分割が発生するパターン
	{ 
		auto splitONode = [&]( std::shared_ptr<unsigned char> *_targetKey , std::shared_ptr<optr> *_targetDataOptr  ,std::shared_ptr<ONode> *_targetONode ) // targetKeyとtargetONodeは入出力引数となる
		{
			std::cout << "ノード分割が発生します" << "\n";
			ViewItemSet viewItemSet; // 分裂中央のコントロールが面倒なので仮想的にサイズが一つ大きいアイテムセットを用意する
			viewItemSet.importItemSet( _itemSet ); // 仮想アイテムセットに既存のアイテムセット情報を取り込む

			// 一旦仮想アイテムセットに対象要素を追加する
			viewItemSet._key.at( DEFAULT_KEY_COUNT ) = *_targetKey; // 繰り越したキーの挿入
			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;
			// 論理アイテムセットのキーをソートする
			std::sort( viewItemSet._key.begin() , viewItemSet._key.end() , []( std::shared_ptr<unsigned char> a, std::shared_ptr<unsigned char> b){
				return memcmp( a.get() , b.get() , KEY_SIZE ) <= 0;
			});	 // viewItemSetのメソッドとした方が良い


			unsigned short keyInsertedIndex = 0; // 最終的にキーを挿入した位置を得る
			for( int i=0; i<viewItemSet._key.size(); i++ )
			{
				if( memcmp( viewItemSet._key.at(i).get(), targetKey.get(), KEY_SIZE ) == 0 ){
					keyInsertedIndex = i;
					break;
				}
			}

			viewItemSet.moveInsertDataOptr( keyInsertedIndex, *_targetDataOptr ); // dataOptrを挿入する

			if( *_targetONode != nullptr ){ // 追加対象がリーフノードでない限りは追加される
				viewItemSet.moveInsertChild( keyInsertedIndex + 1 , *_targetONode );
			}

			std::shared_ptr<unsigned char> separatorKey = viewItemSet._key.at( separatorKeyIndex ); // これは親ノードへの追加対象となる
			std::shared_ptr<optr> separatorDataOptr = viewItemSet._dataOPtr.at( separatorKeyIndex ); // これも親ノードへの追加対象となる
			// このアイテムセットは右子ノードとなる　分割対象の中央右ノードを格納する		
			std::shared_ptr<ONode> splitONode = std::make_shared<ONode>( _oMemoryManager ); // (新たな右子ノード)  親ノードへの挿入対象となる 子ノードとして作成する

			
			// エラーでそう 
			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	{ // キーの挿入
				splitONode->itemSet()->key( i , viewItemSet._key.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			for( int i=0; i<(DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1;i++)	{ // データポインタの挿入
				splitONode->itemSet()->dataOptr( i , viewItemSet._dataOPtr.at(i+separatorKeyIndex+1) );
			}	
			splitONode->itemSet()->dataOptrCount( (DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1 );

			splitONode->parent( this->parent() );

			// 本アイテムセットは左子ノードとなる 分割対象の中央左のキーを格納する
			for( int i=0; i<separatorKeyIndex; i++)
				_itemSet->key( i , viewItemSet._key.at(i) );
			_itemSet->keyCount(separatorKeyIndex);
			for( int i=0; i<separatorKeyIndex; i++) // データポインタの挿入
				_itemSet->dataOptr( i , viewItemSet._dataOPtr.at(i) );
			_itemSet->dataOptrCount(separatorKeyIndex);


			if( _itemSet->childCount() > 0 ) // アイテムセットに子供ノードが存在する場合は分割したノードに分ける
			{
				int center = (viewItemSet._child.size() % 2 == 0) ? (viewItemSet._child.size()/2) : ((viewItemSet._child.size()/2) );
				for( int i=0; i<center; i++ ){
					_itemSet->child( i , viewItemSet._child.at(i) ); // 分割後左ノード
				}
				_itemSet->childCount( center );

				for( int i=center; i<viewItemSet._child.size(); i++ ){
					splitONode->itemSet()->child( i - center, viewItemSet._child.at(i) );
					splitONode->itemSet()->childCount(i-center+1);

					if( splitONode->itemSet()->child( i - center ) != nullptr ){
						splitONode->itemSet()->child( i - center )->parent( splitONode ); // 分割後右ノード
					}
				}
				splitONode->itemSet()->childCount( viewItemSet._child.size() - center );
			}

			*_targetKey = separatorKey;
			*_targetDataOptr = separatorDataOptr;
			*_targetONode = splitONode; // 新たに作成したONode

		};

		splitONode( &targetKey , &targetDataOptr ,&targetONode );


		std::shared_ptr<ONode> parentONode = this->parent();
		unsigned char oAddrZero[5]; memset( oAddrZero, 0x00 , sizeof(oAddrZero) );

		
		if( memcmp( parentONode->itemSet()->Optr()->addr(), oAddrZero, 5  )  == 0 )  // 自身がルートノード場合
		{
			//ONode* newRootNode = new ONode( _oMemoryManager ); // 新たなルートノードの作成
			std::shared_ptr<ONode> newRootNode = std::shared_ptr<ONode>( new ONode(_oMemoryManager) );

			/* ルートノード情報の上書き */
			std::cout << "[ @ ] (Btree::ルートノードが上書きされました)" << "\n";
			std::shared_ptr<unsigned char> metaHeadOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] ); memset( metaHeadOAddr.get() , 0x00 , 5 );
			std::shared_ptr<optr> metaHeadOptr = std::shared_ptr<optr>( new optr(metaHeadOAddr.get() , _oMemoryManager->dataCacheTable()) );
			omemcpy( (*metaHeadOptr) + META_ROOT_NODE_OFFSET ,  newRootNode->itemSet()->Optr()->addr() , NODE_OPTR_SIZE );

			newRootNode->isLeaf( false );

			newRootNode->itemSet()->key( 0 ,targetKey );
			newRootNode->itemSet()->keyCount(1);

			newRootNode->itemSet()->dataOptr( 0 , targetDataOptr );
			newRootNode->itemSet()->dataOptrCount(1);

			newRootNode->itemSet()->child( 0 , shared_from_this() ); // 本Oノードの追加
			newRootNode->itemSet()->childCount( 1 );

			newRootNode->itemSet()->child( 1 ,targetONode ); // 新規に作成したノードの追加
			newRootNode->itemSet()->childCount(2);

			this->parent( newRootNode ); // 左ONodeの親はこれにセット
			targetONode->parent( newRootNode ); // 右ONOdeの親はこれにセット
			newRootNode->parent( nullptr ); // 新たなルートノードの親ノードは0x00(無設定)になる
			
			return newRootNode;  // 必ずリターンする
		}

		// 親ノードが存在する場合
		return parentONode->recursiveAdd( targetKey, targetDataOptr ,targetONode );
	}

	else{ // 単純追加

		_itemSet->key( _itemSet->keyCount() , targetKey );
		_itemSet->keyCount( _itemSet->keyCount() + 1 );
		_itemSet->sortKey();

	
		unsigned short keyInsertedIndex = 0; // キー素挿入した位置を得る
		std::shared_ptr<optr> oKey; std::shared_ptr<unsigned char> uKey = std::shared_ptr<unsigned char>( new unsigned char[20] );
		for( int i=0; i<_itemSet->keyCount(); i++ )
		{
			oKey = _itemSet->key(i); omemcpy( uKey.get(), oKey , KEY_SIZE );
			if( memcmp( uKey.get(), targetKey.get(), KEY_SIZE ) == 0 ){
				keyInsertedIndex = i;
				break;
			}
		}
		//_itemSet->dataOptr( keyInsertedIndex,  targetDataOptr );
		_itemSet->moveInsertDataOptr( keyInsertedIndex , targetDataOptr );
		_itemSet->dataOptrCount( _itemSet->dataOptrCount() + 1 );

		if( targetONode != nullptr )
		{
			_itemSet->moveInsertChild( keyInsertedIndex + 1  , targetONode );
			_itemSet->childCount( _itemSet->keyCount() + 1 );
		}

		
		return nullptr; // 一応
	}

	return nullptr;
}



std::shared_ptr<ONode> ONode::child( unsigned short index )
{
	std::shared_ptr<ONode> ret = _itemSet->child( index );
	if( ret == nullptr ) return nullptr;

	ret->overlayMemoryManager( _oMemoryManager ); // OverlayMemoryManagerをセットする
	return ret;
}


void ONode::overlayMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;
}


std::shared_ptr<OverlayMemoryManager> ONode::overlayMemoryManager()
{
	return _oMemoryManager;
}




void ONode::regist( unsigned char *targetKey , optr *targetDataOptr )
{

	std::unique_ptr<ONode> insertTargetONode = subtreeKeySearch( this , targetKey );
	

}


int ONode::findIndex( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
	std::shared_ptr<optr> keyOptr; int flag;
	for( int i=0; i<_itemSet->keyCount(); i++ ){
		keyOptr = _itemSet->key(i);  omemcpy( rawKey.get(), keyOptr, KEY_SIZE );
		flag = memcmp( targetKey.get() , rawKey.get(), KEY_SIZE );

		if( flag == 0 ) return i;
	}
	return -1;
}


std::shared_ptr<ONode> ONode::remove( std::shared_ptr<unsigned char> targetKey )
{
	int index = findIndex( targetKey );
	if( index < 0 ) return nullptr;

	unsigned char addrZero[5]; memset( addrZero, 0x00 , sizeof(addrZero) );
	if( _itemSet->keyCount() > 1 || memcmp( parent()->itemSet()->Optr()->addr() , addrZero, sizeof(addrZero) ) == 0 )  // 単純削除のケース
	{ 
		std::cout << "単純削除" << "\n";
		_itemSet->remove( index ); 
		return nullptr;
	}

	return this->parent()->underflow( shared_from_this() ); // アンダーフロー発生を通知
	
	/*
		削除パターン

		[ 削除対象がリーフノードの存在 ]
			1. 削除対象が属するONodeが要素を2つ以上(削除対象を含む)持っている場合
			   -> 単純削除
			2. 削除対象が属するONodeが要素が1つ以下(削除対象のみ)の場合 && 兄妹ノードから持って来れる要素がある

			3. 削除対象が属するONodeが要素が1つ以下(削除対象のみ)の場合 && 兄妹ノードから持って来れる要素がない



		[ 削除対処がリーフ以外に存在 ]
	*/

}



// 通知元の子ノードを通知するべきか？
std::shared_ptr<ONode> ONode::underflow( std::shared_ptr<ONode> sourceONode )
{
	std::cout << "\x1b[33m" << "underflowが発生しました" << "\x1b[39m" << "\n";
	// とりあえずsourceONodeを元に,削除対象のONodeのインデックスを逆引きする(非効率)
	int index = -1;
	for( int i=0; i<=_itemSet->childCount() - 1; i++ ){
		index = ( memcmp( sourceONode->itemSet()->Optr()->addr(), this->_itemSet->child(i)->itemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) return nullptr;	


	/* キー移動 */
	// 左右兄弟を検索してキー移動が可能であれば移動する
	// 検索対象は対象と隣接している兄弟(対象からインデックス差が±1)ONodeだけ　
	std::shared_ptr<ONode> targetONode = _itemSet->child(index);
	std::shared_ptr<ONode> leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	std::shared_ptr<ONode> rightChildONode = this->child(index+1);

	
	
	// 兄弟ノードから移動できるキーが存在するか確認する
	if( leftChildONode != nullptr && leftChildONode->itemSet()->keyCount() >= 2 ){
		std::cout << "左兄弟ノードからキーを移動" << "\n";
		sourceONode->itemSet()->key( 0 , _itemSet->rawKey(index-1) );
		sourceONode->itemSet()->keyCount(1);
		sourceONode->itemSet()->dataOptr( 0 , _itemSet->dataOptr(index-1) );
		sourceONode->itemSet()->dataOptrCount(1);

		_itemSet->key( index-1, leftChildONode->itemSet()->rawKey( leftChildONode->itemSet()->keyCount() - 1 ) ); // 最後尾を取得
		leftChildONode->itemSet()->moveDeleteKey( leftChildONode->itemSet()->keyCount() - 1 );
		_itemSet->dataOptr( index-1 , leftChildONode->itemSet()->dataOptr(leftChildONode->itemSet()->dataOptrCount() - 1 ) );
		leftChildONode->itemSet()->moveDeleteDataOptr( leftChildONode->itemSet()->dataOptrCount() - 1 );
		return nullptr;
	}
	if( rightChildONode != nullptr && rightChildONode->itemSet()->keyCount() >= 2 )
	{
		std::cout << "右兄弟ノードからキーを移動" << "\n";
		// 右ノードからキー移動
		sourceONode->itemSet()->key( 0 , _itemSet->rawKey(index) );
		sourceONode->itemSet()->keyCount(1);
		sourceONode->itemSet()->dataOptr( 0 , _itemSet->dataOptr(index) );
		sourceONode->itemSet()->dataOptrCount(1);

		_itemSet->key( index, rightChildONode->itemSet()->rawKey( 0 ) ); // 最後尾を取得
		rightChildONode->itemSet()->moveDeleteKey( 0 );
		_itemSet->dataOptr( index , rightChildONode->itemSet()->dataOptr(0) );
		rightChildONode->itemSet()->moveDeleteDataOptr(0);
	
		return nullptr;
	}


	// underflowによるキー移動ができなかったので,マージ処理を行う
	/* マージ */ 
	return merge( index );
}



std::shared_ptr<ONode> ONode::merge( std::shared_ptr<ONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=_itemSet->childCount() - 1; i++ ){
		index = ( memcmp( sourceONode->itemSet()->Optr()->addr(), this->_itemSet->child(i)->itemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) nullptr;
	return this->merge( index );
}


std::shared_ptr<ONode> ONode::merge( unsigned short index ) // mergeが呼び出される時,兄弟ノードの各要素数は1
{
	// マージ前処理
	// 子ノードに追加して本ノードを削除対象とする
	std::cout << "\x1b[33m" << "underflow(マージ)が発生しました" << "\x1b[39m" << "\n";
	if( index == -1 ) return nullptr;

	std::shared_ptr<ONode> targetONode = _itemSet->child(index);
	std::shared_ptr<ONode> leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	std::shared_ptr<ONode> rightChildONode = this->child(index+1);

	printf("左兄弟ノード %p\n", leftChildONode.get() );
	printf("右兄弟ノード %p\n", rightChildONode.get() );
	std::cout << "index :: " << index << "\n";

	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );

	// マージの種類
	// 1. 兄弟ノードに要素が2つ以上ある場合
	if( _itemSet->keyCount() >= 2 )
	{
		// indexの箇所の子ノードを解消する
		if( leftChildONode != nullptr )	
		{
			std::cout << "左兄弟ノードマージ" << "\n";
			std::cout << "index :: " << index << "\n";
			_itemSet->moveDeleteChild( index );

			leftChildONode->itemSet()->key( leftChildONode->itemSet()->keyCount() ,_itemSet->rawKey(index-1) );
			leftChildONode->itemSet()->keyCount( leftChildONode->itemSet()->keyCount() + 1 );
			_itemSet->moveDeleteKey(index-1);

			leftChildONode->itemSet()->dataOptr( leftChildONode->itemSet()->dataOptrCount(), _itemSet->dataOptr(index-1) ); 
			leftChildONode->itemSet()->dataOptrCount( leftChildONode->itemSet()->dataOptrCount() + 1 );

			//_itemSet->moveInsertChild( _itemSet->childCount(), leftChildONode->itemSet()->child( leftChildONode->itemSet()->childCount()) );
			//_itemSet->childCount( _itemSet->childCount() + 1 );
			//leftChildONode->itemSet()->moveDeleteChild( leftChildONode->itemSet()->childCount() );
			return nullptr; //　ルートノードを変更することなく終了
		}

		if( rightChildONode != nullptr )
		{
			std::cout << "右兄弟ノードマージ前処理" << "\n";
			std::cout << "index :: " << index << "\n";
			_itemSet->moveDeleteChild( index );

			rightChildONode->itemSet()->moveInsertKey( 0 , _itemSet->rawKey(index) );
			rightChildONode->itemSet()->keyCount( rightChildONode->itemSet()->keyCount() + 1 );
			_itemSet->moveDeleteKey(index);

			rightChildONode->itemSet()->moveInsertDataOptr( 0 , _itemSet->dataOptr(index) );
			rightChildONode->itemSet()->dataOptrCount( rightChildONode->itemSet()->dataOptrCount() );

			//_itemSet->moveInsertChild( _itemSet->childCount() , rightChildONode->itemSet()->child(0) );
			//_itemSet->childCount( _itemSet->childCount() + 1 );
			//rightChildONode->itemSet()->moveDeleteChild(0);
			return nullptr;
		}
	}
	

	if( leftChildONode != nullptr )
	{
		/* 左再帰マージ前処理 
			1. 左ノードに親ノードの要素を移動させる
			2. 親ノードの要素を削除する
			3. 親ノード(中身は空)に対してマージ処理を行う
		*/
		std::cout << "左再帰マージ前処理" << "\n";
		leftChildONode->itemSet()->moveInsertKey( leftChildONode->itemSet()->keyCount() , itemSet()->rawKey(index-1) ); // 0でもいい
		leftChildONode->itemSet()->keyCount( leftChildONode->itemSet()->keyCount() + 1);
		itemSet()->moveDeleteKey(index-1);

		leftChildONode->itemSet()->moveInsertDataOptr( leftChildONode->itemSet()->dataOptrCount() , itemSet()->dataOptr(index-1) );
		leftChildONode->itemSet()->dataOptrCount( leftChildONode->itemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index-1);

		itemSet()->moveDeleteChild(index); // 子ノードの削除

		if( memcmp( parent()->itemSet()->Optr()->addr(), addrZero, sizeof(addrZero)  ) == 0 ) {
			_itemSet->child(0)->parent(nullptr);
			return _itemSet->child(0);
		}
		//leftChildONode->itemSet()->moveInsertChild( leftChildONode->itemSet()->childCount(), itemSet()->child(index) );
		//leftChildONode->itemSet()->childCount( leftChildONode->itemSet()->childCount() + 1 );
		//leftChildONode->itemSet()->child( leftChildONode->itemSet()->childCount() - 1)->parent( leftChildONode );

		return this->parent()->recursiveMerge( shared_from_this() );
	}

	if( rightChildONode != nullptr )
	{
		std::cout << "右再帰マージ前処理" << "\n";

		rightChildONode->itemSet()->moveInsertKey( 0 , itemSet()->rawKey(index) ); // 0でもいい？
		rightChildONode->itemSet()->keyCount( rightChildONode->itemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index);

		rightChildONode->itemSet()->moveInsertDataOptr( 0 , itemSet()->dataOptr(index) );
		rightChildONode->itemSet()->dataOptrCount( rightChildONode->itemSet()->dataOptrCount() + 1 ); 
		itemSet()->moveDeleteDataOptr(index);

		itemSet()->moveDeleteChild(index);
		// ここで本ノードは空になっているはず

		if( memcmp( parent()->itemSet()->Optr()->addr(), addrZero, sizeof(addrZero)  ) == 0 ){
			_itemSet->child(0)->parent(nullptr);
			return _itemSet->child(0);
		} 
		//rightChildONode->itemSet()->moveInsertChild( 0 , itemSet()->child(index) );
		//rightChildONode->itemSet()->childCount( rightChildONode->itemSet()->childCount() + 1 );
		//rightChildONode->itemSet()->child(0)->parent( rightChildONode );
		//itemSet()->moveDeleteChild(index);

		return this->parent()->recursiveMerge( shared_from_this() );
	}

	return nullptr; 
}



std::shared_ptr<ONode> ONode::recursiveMerge( std::shared_ptr<ONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=_itemSet->childCount() - 1; i++ ){
		index = ( memcmp( sourceONode->itemSet()->Optr()->addr(), this->_itemSet->child(i)->itemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) nullptr;
	return this->recursiveMerge( index );
}




// これが呼び出される時は必ず親の要素(キー)数が1こである
std::shared_ptr<ONode> ONode::recursiveMerge( unsigned short index ) // mergeが呼び出されて後に必ず呼び出される 
{
	std::cout << "再帰マージが実行されます" << "\n";
	std::cout << "index :: " << index << "\n";
	std::cout << "keyCount() :: " << _itemSet->keyCount() << "\n";
	if( _itemSet->keyCount() <= 0 ){ 
		std::cout << "ここでは子ノードの数は一つだけのはず child Count :: " << _itemSet->childCount() << "\n";
		_itemSet->child(0)->parent(nullptr);
		return _itemSet->child(0); // 新たなルートノードを返却する
	}

	if( index == -1 ) return nullptr;

	std::shared_ptr<ONode> targetONode = _itemSet->child(index);
	std::shared_ptr<ONode> leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	std::shared_ptr<ONode> rightChildONode = this->child(index+1);



	if( leftChildONode != nullptr && leftChildONode->itemSet()->keyCount() >= 2 ) // 兄弟左ノードから親ノード要素を押し出す
	{
		std::cout << "左移動再帰マージ" << "\n";
		// 再帰追加の際はindex要素はまだ死んでいない	
		targetONode->itemSet()->key( 0 , _itemSet->rawKey(0) );
		targetONode->itemSet()->keyCount( targetONode->itemSet()->keyCount() + 1 );
		_itemSet->keyCount(0);
		targetONode->itemSet()->dataOptr( 0 , _itemSet->dataOptr(0) );
		_itemSet->dataOptrCount(0);

		_itemSet->key( 0 , leftChildONode->itemSet()->rawKey(leftChildONode->itemSet()->keyCount()) );  // 最後尾
		_itemSet->keyCount(1);
		leftChildONode->itemSet()->moveDeleteKey( leftChildONode->itemSet()->keyCount() );
		_itemSet->dataOptr( 0 , leftChildONode->itemSet()->dataOptr(leftChildONode->itemSet()->dataOptrCount()) );
		_itemSet->dataOptrCount(1);
		leftChildONode->itemSet()->moveDeleteDataOptr(leftChildONode->itemSet()->dataOptrCount());

		targetONode->itemSet()->moveInsertChild( 0, leftChildONode->itemSet()->child(leftChildONode->itemSet()->childCount()) );
		targetONode->itemSet()->childCount( targetONode->itemSet()->childCount() + 1 );
		targetONode->itemSet()->child( 0 )->parent( targetONode );
		//printf("%p\n", targetONode->itemSet()->child( targetONode->itemSet()->childCount()).get() );
		leftChildONode->itemSet()->moveDeleteChild( leftChildONode->itemSet()->childCount() );
		return nullptr;

	}

	if( rightChildONode != nullptr && rightChildONode->itemSet()->keyCount() >= 2 ) // 兄弟右ノードから親ノードの要素を押し出す
	{
		std::cout << "右移動再帰マージ" << "\n";
		// 再帰追加の際はindex要素はまだ死んでいない	
		targetONode->itemSet()->key( 0 , _itemSet->rawKey(0) );
		targetONode->itemSet()->keyCount( targetONode->itemSet()->keyCount() + 1 );
		_itemSet->keyCount(0);
		targetONode->itemSet()->dataOptr( 0 , _itemSet->dataOptr(0) );
		_itemSet->dataOptrCount(0);

		_itemSet->key( 0 , rightChildONode->itemSet()->rawKey(0) ); 
		_itemSet->keyCount(1);
		rightChildONode->itemSet()->moveDeleteKey(0);
		_itemSet->dataOptr( 0 , rightChildONode->itemSet()->dataOptr(0) );
		_itemSet->dataOptrCount(1);
		rightChildONode->itemSet()->moveDeleteDataOptr(0);

		targetONode->itemSet()->moveInsertChild( targetONode->itemSet()->childCount(), rightChildONode->itemSet()->child(0) );
		targetONode->itemSet()->childCount( targetONode->itemSet()->childCount() + 1 );
		targetONode->itemSet()->child( targetONode->itemSet()->childCount() - 1 )->parent( targetONode );
		//printf("%p\n", targetONode->itemSet()->child( targetONode->itemSet()->childCount()).get() );
		rightChildONode->itemSet()->moveDeleteChild(0);
		return nullptr;
	}

	// 最終手段
	if( leftChildONode != nullptr )
	{
		std::cout << "トップレベル左再帰マージ" << "\n";
		std::cout << "index :: " << index << "\n";
		leftChildONode->itemSet()->moveInsertKey( leftChildONode->itemSet()->keyCount(), _itemSet->rawKey(index-1) );
		leftChildONode->itemSet()->keyCount( leftChildONode->itemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index-1); 

		leftChildONode->itemSet()->moveInsertDataOptr( leftChildONode->itemSet()->dataOptrCount() , itemSet()->dataOptr(index-1) );
		leftChildONode->itemSet()->dataOptrCount( leftChildONode->itemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index-1);

		leftChildONode->itemSet()->moveInsertChild( leftChildONode->itemSet()->childCount() , targetONode->itemSet()->child(0) );
		leftChildONode->itemSet()->childCount( leftChildONode->itemSet()->childCount() + 1 );
		leftChildONode->itemSet()->child( leftChildONode->itemSet()->childCount() - 1 )->parent( leftChildONode );

		_itemSet->moveDeleteChild(index);

		if( _itemSet->keyCount() <= 0 ) return _itemSet->child(0); // ルートノード変更
		return nullptr; // ルートノードの変更は発生しない

	}

	if( rightChildONode != nullptr )
	{
		std::cout << "トップレベル右再帰マージ" << "\n";
		rightChildONode->itemSet()->moveInsertKey( 0 , itemSet()->rawKey(index) ); // 0でもいい？
		rightChildONode->itemSet()->keyCount( rightChildONode->itemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index);

		rightChildONode->itemSet()->moveInsertDataOptr( 0 , itemSet()->dataOptr(index) );
		rightChildONode->itemSet()->dataOptrCount( rightChildONode->itemSet()->dataOptrCount() + 1 ); 
		itemSet()->moveDeleteDataOptr(index);
		// ここで本ノードは空になっているはず

		rightChildONode->itemSet()->moveInsertChild( 0 , targetONode->itemSet()->child(0) );
		rightChildONode->itemSet()->childCount( rightChildONode->itemSet()->childCount() + 1 );
		rightChildONode->itemSet()->child( rightChildONode->itemSet()->childCount() - 1 )->parent( rightChildONode );

		_itemSet->moveDeleteChild(index);

		if( _itemSet->keyCount() <= 0 ) return _itemSet->child(0);
		return nullptr;
	}

	return nullptr;
}



std::shared_ptr<optr> ONode::subtreeFind( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<ONode> candidateChild;
	

	std::shared_ptr<optr> keyOptr;
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[5] );
	int flag;

	for( int i=0; i<_itemSet->keyCount(); i++ )
	{
		keyOptr = _itemSet->key(i); omemcpy( rawKey.get(), keyOptr , 5 );

		flag = memcmp( targetKey.get() , rawKey.get() , 5 );


		if( flag < 0 ){
			candidateChild = _itemSet->child(i);
			if( candidateChild == nullptr ) return nullptr;
			goto direct;
		}
		else if( flag == 0 ){
			return _itemSet->dataOptr(i);
		}
	}


	candidateChild = _itemSet->child( _itemSet->childCount() -1 );
	if( candidateChild == nullptr ) return nullptr;

	direct:
		return candidateChild->subtreeFind( targetKey );
}




std::unique_ptr<ONode> ONode::subtreeKeySearch( ONode* targetONode ,unsigned char *targetKey )
{
	if( targetONode->isLeaf() ) return std::unique_ptr<ONode>(targetONode);

	int keyCount = targetONode->itemSet()->keyCount();
	int i=0; int course;

	for(i=0; i<keyCount; i++)
	{
		course = ocmp( targetKey ,targetONode->itemSet()->key(i).get() , KEY_SIZE );
		if( course <= 0 ) break;
	}


	if( course == 0 ) return std::unique_ptr<ONode>( targetONode );
	if( course < 0 ) return subtreeKeySearch( targetONode->itemSet()->child(i).get() , targetKey ); 

	return subtreeKeySearch( targetONode->itemSet()->child(keyCount-1).get() , targetKey );
}




















OBtree::OBtree( std::shared_ptr<OverlayMemoryManager> oMemoryManager, std::shared_ptr<ONode> rootNode )
{
	if( rootNode == nullptr )
	{
		// インデックスファイルが初期化されているか調べる
		std::shared_ptr<unsigned char> metaHeadOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] );
		memset( metaHeadOAddr.get() , 0x00 , 5 );
		std::shared_ptr<optr>	metaHeadOptr = std::shared_ptr<optr>( new optr( metaHeadOAddr.get() , oMemoryManager->dataCacheTable() ) );
		
		if( ocmp( metaHeadOptr.get() , (unsigned char *)FORMAT_CODE , 20 ) != 0 ) // (初期化されていない)インデックスファイルを初期化する 
		{
			std::cout << "Btreeインデックスファイルの初期化されていない" << "\n";
			// ここで確保する領域がファイル先頭であることが前提 それ以外だとエラーが発生する
			std::shared_ptr<optr>	newMetaHeadOptr = oMemoryManager->allocate( 100 ); // Meta領域のサイズ分確保する
			
			_rootONode = std::shared_ptr<ONode>( new ONode(oMemoryManager) );  // meta領域に続いて領域を作成する
			omemcpy( (*metaHeadOptr) + META_ROOT_NODE_OFFSET , _rootONode->itemSet()->Optr()->addr() , NODE_OPTR_SIZE ); // ルートノードのセット

			omemcpy( metaHeadOptr.get() , (unsigned char *)(FORMAT_CODE) , 20 ); // フォーマットコードを設置する
			return;	
		}


		std::cout << "Btreeインデックスファイルの初期は既に完了しています" << "\n";
		// Meta情報を記録する領域を作成する
		std::shared_ptr<unsigned char> rootNodeOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] );
		omemcpy( rootNodeOAddr.get() , (*metaHeadOptr) + META_ROOT_NODE_OFFSET , 5 );

		std::shared_ptr<optr> rootNodeOptr = std::shared_ptr<optr>( new optr(rootNodeOAddr.get(), oMemoryManager->dataCacheTable() ));
		_rootONode = std::shared_ptr<ONode>( new ONode( oMemoryManager ,rootNodeOptr) );

		return;	
	}

	_rootONode = rootNode;

}




void OBtree::rootONode( std::shared_ptr<ONode> target )
{
	_rootONode = target;
}




const std::shared_ptr<ONode> OBtree::rootONode()
{
	return _rootONode;
}




void OBtree::add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr )
{
	std::shared_ptr<ONode> deepestONode;

	std::shared_ptr<ONode> currentONode = _rootONode;

	while( (currentONode->itemSet()->childCount() >= 1 ) )
	{
		std::shared_ptr<optr> keyOptr;
		std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		for( int i=0; i<currentONode->itemSet()->childCount(); i++ )
		{
			keyOptr = currentONode->itemSet()->key(i);
			omemcpy( rawKey.get() , keyOptr , KEY_SIZE );

			if( memcmp( targetKey.get(), rawKey.get() , KEY_SIZE ) < 0 ){
				currentONode = currentONode->child(i);
				goto direct;
			}
		}

		currentONode = currentONode->child( currentONode->itemSet()->childCount()-1 );

		direct:;
	}

	
	deepestONode = currentONode;
	
	std::shared_ptr<ONode> newRootNode;
	newRootNode = deepestONode->recursiveAdd( targetKey , dataOptr , nullptr );
	if( newRootNode != nullptr )
		_rootONode = newRootNode;
	return;
}


void OBtree::remove( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<ONode> deepestONode;
	std::shared_ptr<ONode> currentONode = _rootONode;
	while( (currentONode->itemSet()->childCount() >= 1 ) )
	{
		std::shared_ptr<optr> keyOptr;
		std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		for( int i=0; i<currentONode->itemSet()->childCount(); i++ )
		{
			keyOptr = currentONode->itemSet()->key(i);
			omemcpy( rawKey.get() , keyOptr , KEY_SIZE );

			if( memcmp( targetKey.get(), rawKey.get() , KEY_SIZE ) < 0 ){
				currentONode = currentONode->child(i);
				goto direct;
			}
		}

		currentONode = currentONode->child( currentONode->itemSet()->childCount()-1 );

		direct:;
	}

	deepestONode = currentONode;

	std::cout << "\n\n----------------------------------" << "\n";
	std::shared_ptr<ONode> newRootNode = nullptr;
	newRootNode = deepestONode->remove( targetKey );
	if( newRootNode != nullptr ) // ルートノードを変更する
		_rootONode = newRootNode;
	return;
}



std::shared_ptr<optr> OBtree::find( std::shared_ptr<unsigned char> targetKey )
{
	return _rootONode->subtreeFind( targetKey );
}


int OBtree::printONode( std::shared_ptr<ONode> targetONode )
{
	std::cout << "\n\n============================================" << "\n";
	std::cout << "| [ "; targetONode->itemSet()->Optr()->printAddr(); std::cout << " ]\n";
	std::cout << "| [ 親ノードアドレス ] :: ";
	targetONode->parent()->itemSet()->Optr()->printAddr(); std::cout << "\n";

	std::cout << "| [ キー個数 ] :: " << targetONode->itemSet()->keyCount() << "\n";
	for( int i=0; i<targetONode->itemSet()->keyCount(); i++ )
	{
		std::shared_ptr<unsigned char> rawKey = targetONode->itemSet()->rawKey(i);
		std::shared_ptr<optr> oKey = targetONode->itemSet()->key(i);
		std::cout << "|　 (" << i << ") "; oKey->printValueContinuously(20);  std::cout << "[";
		for( int i=0; i<20; i++ ) printf("%c", rawKey.get()[i]);
		std::cout << "]\n";
	}


	std::cout << "| [ データOポインターの個数 ] :: " << targetONode->itemSet()->dataOptrCount() << "\n";

	std::cout << "| [ 子ノード個数 ] :: " << targetONode->itemSet()->childCount() << "\n";
	for( int i=0; i<targetONode->itemSet()->childCount(); i++ )
	{
		std::shared_ptr<ONode> cON = targetONode->itemSet()->child(i);
		std::cout << "|　 (" << i << ")" ;
		cON->itemSet()->Optr()->printAddr(); std::cout << "\n";
	}
	
	std::cout << "| [ リーフノード ] :: " << targetONode->isLeaf()  << "\n";
	std::cout << "============================================\n\n" << "\n";
}




int OBtree::printSubTree( std::shared_ptr<ONode> subtreeRoot )
{

	std::cout << "\n\n============================================" << "\n";
	std::cout << "| [ "; subtreeRoot->itemSet()->Optr()->printAddr(); std::cout << " ]\n";
	std::cout << "| [ 親ノードアドレス ] :: ";
	subtreeRoot->parent()->itemSet()->Optr()->printAddr(); std::cout << "\n";

	std::cout << "| [ キー個数 ] :: " << subtreeRoot->itemSet()->keyCount() << "\n";
	for( int i=0; i<subtreeRoot->itemSet()->keyCount(); i++ )
	{
		std::shared_ptr<unsigned char> rawKey = subtreeRoot->itemSet()->rawKey(i);
		std::shared_ptr<optr> oKey = subtreeRoot->itemSet()->key(i);
		std::cout << "|　 (" << i << ") "; oKey->printValueContinuously(20);  std::cout << "[";
		for( int i=0; i<20; i++ ) printf("%c", rawKey.get()[i]);
		std::cout << "]\n";
	}


	std::cout << "| [ データOポインターの個数 ] :: " << subtreeRoot->itemSet()->dataOptrCount() << "\n";

	std::cout << "| [ 子ノード個数 ] :: " << subtreeRoot->itemSet()->childCount() << "\n";
	for( int i=0; i<subtreeRoot->itemSet()->childCount(); i++ )
	{
		std::shared_ptr<ONode> cON = subtreeRoot->itemSet()->child(i);
		std::cout << "|　 (" << i << ")" ;
		cON->itemSet()->Optr()->printAddr(); std::cout << "\n";
	}
	
	std::cout << "| [ リーフノード ] :: " << subtreeRoot->isLeaf()  << "\n";
	std::cout << "============================================\n\n" << "\n";



	for( int i=0; i<subtreeRoot->itemSet()->childCount(); i++ )
	{
		OBtree::printSubTree( subtreeRoot->child(i) );
	}

	return 0;
}







} // close miya_db namespace
