#include "btree.h"
#include "../page_table/optr_utils.h"

#include "../page_table/overlay_memory_manager.h"

namespace miya_db
{





/*
void ViewItemSet::importItemSet( ONodeItemSet *itemSet )
{
	std::array<unsigned char*, DEFAULT_KEY_COUNT> *keySorce = itemSet->exportKeyArray();
	std::copy( keySorce->begin() ,keySorce->end() , _key->begin() ); // 上書きで大丈夫

	std::array<ONode*, DEFAULT_CHILD_COUNT> *childSorce = itemSet->exportChildArray();
	std::copy( childSorce->begin() ,childSorce->end(), _child->begin() ); // 上書き

	return;
}
*/


void ViewItemSet::importItemSet( std::shared_ptr<ONodeItemSet> itemSet )
{
	std::array< std::shared_ptr<unsigned char>, DEFAULT_KEY_COUNT> *keySorce = itemSet->exportKeyArray(); // キーを一旦配列として書き出す
	std::copy( keySorce->begin() ,keySorce->end() , _key.begin() ); // 上書きで大丈夫

	std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT> *childSorce = itemSet->exportChildArray(); // 子ノードを一旦配列として書き出す
	std::copy( childSorce->begin() ,childSorce->end(), _child.begin() ); // 上書き

	return;
}




void ViewItemSet::moveInsert( unsigned short index ,std::shared_ptr<ONode> target )
{
	for( int i = _child.size()-1; i > index; i-- )
		_child.at(i) = _child.at(i-1);

	_child.at(index) = target;
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


std::shared_ptr<optr> ONodeItemSet::parent()
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
	if( index > (childCount()) ) return nullptr;
			
	std::shared_ptr<optr> childHead = *_optr + ( CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE) );
	//std::shared_ptr<optr> sChildHead = std::move( childHead ); // 仮で無理やり変換しておく

	//return std::make_shared<ONode>( new ONode( sChildHead ) );
	
	unsigned char oAddr[5];
	omemcpy( oAddr , childHead , 5 );

	std::shared_ptr<optr> childOptr = std::make_shared<optr>( oAddr, childHead->cacheTable() );
	
	return std::make_shared<ONode>( childOptr);

	//return std::make_shared<optr>( oAddr , _optr->cacheTable() );
	


	//return std::make_shared<ONode>( childHead );
	//return nullptr;
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




void ONodeItemSet::moveInsert( unsigned short index , std::shared_ptr<ONode> targetONode )
{
	for( int i = childCount(); i > index; i-- )
		child(i, child(i-1) );

	child(index, targetONode );
}
/* -------------------------------------------------------------- */




/*
	---------------- データオーバレイポインタ操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::dataOptrCount()
{
	return static_cast<unsigned short>( (*_optr + DAYA_OPTR_ELEMENT_OFFSET )->value() );
}

std::shared_ptr<optr> ONodeItemSet::dataOptr( unsigned short index )
{
	if( index > (dataOptrCount()) ) return nullptr;

	return *_optr + ( DAYA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*DATA_OPTR_SIZE) );
}

/* -------------------------------------------------------------- */





/*
std::array<optr*, DEFAULT_KEY_COUNT> *ONodeItemSet::exportKeyOptrArray()
{
	std::array<optr*, DEFAULT_KEY_COUNT> *ret = new std::array<optr*, DEFAULT_KEY_COUNT>;

	for( int i=0; i<keyCount(); i++ )
		ret->at(i) = key(i).get();

	return ret;
}
*/



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















ONode::ONode( std::shared_ptr<optr> baseOptr ) // これではメモリマネージャーがセットされないので必ずONodeを介すこと
{	
	_itemSet = std::make_shared<ONodeItemSet>( baseOptr );
}



ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;

	std::cout << "allocate before" << "\n";
	std::shared_ptr<optr> baseOptr = oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // 新規作成の場合
	std::cout << "ONode Allocated With -> "; baseOptr->printAddr(); std::cout << "\n";
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
		std::cout << "nullptr 初期化"	 << "\n";
		unsigned char *tmp = new unsigned char[NODE_OPTR_SIZE]; memset( tmp , 0x0000000000, sizeof(NODE_OPTR_SIZE) );
		omemcpy( _itemSet->Optr() , tmp , NODE_OPTR_SIZE );
		return;
	}

	std::cout << " =============  parent( target ) called  ===================== " << "\n";
	target->itemSet()->Optr()->printAddr(); std::cout << "\n";

	omemcpy( _itemSet->Optr() , target->itemSet()->Optr()->addr() , NODE_OPTR_SIZE );

	return;
}


/*
void ONode::parent( ONode* target )
{
	if( target == nullptr )
	{
		unsigned char *tmp = new unsigned char[NODE_OPTR_SIZE]; memset( tmp , 0x0000000000, sizeof(NODE_OPTR_SIZE) );
		omemcpy( _itemSet->Optr() , tmp , NODE_OPTR_SIZE );
	}

	omemcpy( _itemSet->Optr() , target->itemSet()->Optr() , NODE_OPTR_SIZE );
	return;
}
*/



std::shared_ptr<ONodeItemSet> ONode::itemSet()
{
	return _itemSet;
}





std::shared_ptr<ONode> ONode::recursiveAdd( std::shared_ptr<unsigned char> targetKey, std::shared_ptr<ONode> targetONode )
{
	std::cout << "add() called" << "\n";

	if( _itemSet->keyCount() >= DEFAULT_KEY_COUNT ) // ノードの分割が発生するパターン
	{ 
		std::cout << "----------- DO ONode Splits --------------" << "\n";
		auto splitONode = [&]( std::shared_ptr<unsigned char> *_targetKey , std::shared_ptr<ONode> *_targetONode ) // targetKeyとtargetONodeは入出力引数となる
		{

			std::cout << "check -1" << "\n";

			ViewItemSet viewItemSet; // 分裂中央のコントロールが面倒なので仮想的にサイズが一つ大きいアイテムセットを用意する
			
			std::cout << "< -1 - 0 >" << "\n";

			viewItemSet.importItemSet( _itemSet ); // 仮想アイテムセットに既存のアイテムセット情報を取り込む

			// 一旦仮想アイテムセットに対象要素を追加する
			viewItemSet._key.at( DEFAULT_KEY_COUNT ) = *_targetKey; // 繰り越したキーの挿入

			std::cout << "check 0" << "\n";

			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;


			std::cout << "check 1" << "\n";
				
			// 論理アイテムセットのキーをソートする
			std::sort( viewItemSet._key.begin() , viewItemSet._key.end() , []( std::shared_ptr<unsigned char> a, std::shared_ptr<unsigned char> b){
				return memcmp( a.get() , b.get() , KEY_SIZE ) <= 0;
			});	 // viewItemSetのメソッドとした方が良い



			std::cout << "\x1b[31m"; 
			for( int i=0; i<viewItemSet._child.size()-1; i++ )
			{
				std::shared_ptr<ONode> aa = viewItemSet._child.at(i);
				if( aa == nullptr ) break;
				aa->itemSet()->Optr()->printAddr(); std::cout << "\n";
			}
			std::cout <<"\x1b[m";

			unsigned short keyInsertedIndex = 0;
			for( int i=0; i<viewItemSet._key.size(); i++ )
			{
				if( memcmp( viewItemSet._key.at(i).get(), targetKey.get(), KEY_SIZE ) == 0 ){
					keyInsertedIndex = i;
					break;
				}
			}
			std::cout << "## targetKey ";
			for( int i=0;i<20; i++)
			{
				printf("%c", targetKey.get()[i]);
			} std::cout << "\n";



			if( *_targetONode != nullptr ){ // 追加対象がリーフノードでない限りは追加される
				std::cout << "\x1b[31m" << "IMPORTANT MARK" << "\x1b[m"	 << "\n";
				std::cout << keyInsertedIndex << "\n";
				viewItemSet.moveInsert( keyInsertedIndex + 1 , *_targetONode );
			}
				//viewItemSet._child.at( DEFAULT_CHILD_COUNT ) = *_targetONode; // 対象のキーを挿入した位置に子ノードを挿入しなければならない


			std::cout << "\x1b[31m"; 
			for( int i=0; i<viewItemSet._child.size(); i++ )
			{
				std::shared_ptr<ONode> aa = viewItemSet._child.at(i);
				if( aa == nullptr ) break;
				aa->itemSet()->Optr()->printAddr(); std::cout << "\n";
			}
			std::cout <<"\x1b[m";




	
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
			std::cout << "key count->" << viewItemSet._key.size() << "\n";
			std::shared_ptr<unsigned char> temp;
			for( int i=0;i<viewItemSet._key.size(); i++ )
			{
				temp = viewItemSet._key.at(i); 
				std::cout << "(" << i << ")";
					for( int i=0; i<20; i++)
					{
						printf("%c", temp.get()[i]);
					} std::cout << "\n";
			}
			std::cout << "----------------------------------" << "\n";
			for( int i=0; i<viewItemSet._child.size(); i++ )
			{
				std::shared_ptr<ONode> _temp = viewItemSet._child.at(i);
				if( _temp == nullptr ) break;
				std::cout << "(" << i << ")";
				_temp->itemSet()->Optr()->printValueContinuously(5); std::cout << "\n";
			}
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";


			std::cout << "check 3" << "\n";

			std::shared_ptr<unsigned char> separatorKey = viewItemSet._key.at( separatorKeyIndex ); // これは親ノードへの追加対象となる
		
			std::cout << "check 4" << "\n";
			
			_itemSet->Optr()->printAddr(); std::cout << "\n";
			printf("%p\n", _oMemoryManager.get() );
			// 分割ルーチンスタート 
			// このアイテムセットは右子ノードとなる　分割対象の中央右ノードを格納する		
			std::shared_ptr<ONode> splitONode = std::make_shared<ONode>( _oMemoryManager ); // (新たな右子ノード)  親ノードへの挿入対象となる 子ノードとして作成する
			// メモリーマネージャーだけ渡すと	
	

			std::cout << "\x1b[31m";
			splitONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
			std::cout << "\x1b[m" << "\n";

			std::cout << "check 5" << "\n";

			// エラーでそう
			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	{
				splitONode->itemSet()->key( i , viewItemSet._key.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			splitONode->parent( this->parent() );

			std::cout << ">>>>>>>  " << splitONode->itemSet()->key(0)->value() << "\n";
			std::cout << "## separatorKeyIndex -> " << separatorKeyIndex << "\n";
			std::cout << "## splitONode key count -> " << splitONode->itemSet()->keyCount() << "\n";

		
			// 本アイテムセットは左子ノードとなる 分割対象の中央左のキーを格納する
			for( int i=0; i<separatorKeyIndex; i++)
				_itemSet->key( i , viewItemSet._key.at(i) );
			_itemSet->keyCount(separatorKeyIndex);


			std::cout << "<<<<<<<<<<<<  " << _itemSet->key(0)->value(); std::cout << "\n";
			std::cout << "check 6" << "\n";

			std::cout << "## itemSet key count -> " << _itemSet->keyCount() << "\n";


			if( _itemSet->childCount() > 0 ) // アイテムセットに子供ノードが存在する場合は分割したノードに分ける
			{

				std::cout << "\x1b[31m" << viewItemSet._child.size() << "\x1b[m" << "\n";
				int center = (viewItemSet._child.size() % 2 == 0) ? (viewItemSet._child.size()/2) : ((viewItemSet._child.size()/2) + 1);
				for( int i=0; i<center; i++ ){
					_itemSet->child( i , viewItemSet._child.at(i) );
				}
				_itemSet->childCount( center );
				for( int i=center; i<viewItemSet._child.size(); i++ ){
					splitONode->itemSet()->child( i - center, viewItemSet._child.at(i) );
					splitONode->itemSet()->childCount(i-center+1);

					std::cout << "\x1b[32m";
					std::cout << i -center  << "\n";
					viewItemSet._child.at(i)->itemSet()->Optr()->printAddr(); std::cout << "\n";


					splitONode->child(i-center)->itemSet()->Optr()->printAddr();
					std::cout << "\x1b[m" << "\n";

					if( splitONode->itemSet()->child( i - center ) != nullptr ){
						splitONode->itemSet()->child( i - center )->parent( splitONode );
						std::cout << "\x1b[32m" << "DO" << "\x1b[m" << "\n";
					}
				}
				splitONode->itemSet()->childCount( viewItemSet._child.size() - center );
			}


			*_targetKey = separatorKey;
			*_targetONode = splitONode; // 新たに作成したONode


		
			//return parent()->add( targetKey , targetONode );
		};

		splitONode( &targetKey , &targetONode );

		std::cout << "####################" << "\n";
		for( int i=0; i<20; i++ )
		{
			printf("%c", targetKey.get()[i]);
		} std::cout << "\n";
		std::cout << "####################" << "\n";
		std::cout << targetONode->itemSet()->keyCount() << "\n";
		printf("%c\n", targetONode->itemSet()->key(0)->value() );
		std::cout << "####################" << "\n";


		std::shared_ptr<ONode> parentONode = this->parent();
		unsigned char oAddrZero[5]; memset( oAddrZero, 0x00 , sizeof(oAddrZero) );

		
		if( memcmp( parentONode->itemSet()->Optr()->addr(), oAddrZero, 5  )  == 0 )  // 自身がルートノード場合
		{
			std::cout << "ここです" << "\n";

			//ONode* newRootNode = new ONode( _oMemoryManager ); // 新たなルートノードの作成
			std::shared_ptr<ONode> newRootNode = std::shared_ptr<ONode>( new ONode(_oMemoryManager) );
			newRootNode->isLeaf( true );

			newRootNode->itemSet()->key( 0 ,targetKey );
			newRootNode->itemSet()->keyCount(1);

			//return;

			std::cout << "$$ 1" << "\n";
			std::cout << "*** -> "  <<  newRootNode->itemSet()->childCount() << "\n";

			newRootNode->itemSet()->child( 0 , shared_from_this() ); // 本Oノードの追加
			newRootNode->itemSet()->childCount( 1 );

			std::cout << "hello -> "; this->itemSet()->Optr()->printAddr();  std::cout << "\n";
			std::cout << "++++ "; newRootNode->itemSet()->child(0)->itemSet()->Optr()->printAddr(); std::cout << "\n";

			std::cout << "****** -> " << newRootNode->itemSet()->childCount() << "\n";
			std::cout << "$$ 1-0" << "\n";

			std::cout << "$$ 1-1" << "\n";

			newRootNode->itemSet()->child( 1 ,targetONode ); // 新規に作成したノードの追加
			newRootNode->itemSet()->childCount(2);


			std::cout << "++++++++++++++++++++++++++++++++++" << "\n";
			std::cout << "adder 1 -> "; targetONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
			std::cout << "adder 2 -> "; newRootNode->child(1)->itemSet()->Optr()->printAddr(); std::cout << "\n";
			std::cout << "++++++++++++++++++++++++++++++++++" << "\n";
			newRootNode->child(1)->itemSet()->Optr()->printAddr(); std::cout << "\n";
			std::cout << newRootNode->itemSet()->childCount() << "\n";


			// ()return;

			std::cout << "$$ 2" << "\n";

			printf("%p\n", newRootNode.get() );
			this->parent( newRootNode ); // 左ONodeの親はこれにセット
			
			std::cout << "$$ 2-1" << "\n";

			targetONode->parent( newRootNode ); // 右ONOdeの親はこれにセット

			std::cout << "$$ 3" << "\n";

			newRootNode->parent( nullptr ); // 新たなルートノードの親ノードは0x00(無設定)になる
			
			std::cout << "$$ 4" << "\n";
			return newRootNode;  // 必ずリターンする
		}

		// 親ノードが存在する場合
		std::cout << "................................." << "\n";
		parentONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
		for( int i=0; i<20; i++ )
		{
			printf("%c", targetKey.get()[i]);
		} std::cout << "\n";
		targetONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
		std::cout << "................................." << "\n";
		return parentONode->recursiveAdd( targetKey, targetONode );
	}

	else{ // 単純追加
		std::cout << "キーの単純追加"	 << "\n";


		_itemSet->key( _itemSet->keyCount() , targetKey );
		_itemSet->keyCount( _itemSet->keyCount() + 1 );
		_itemSet->sortKey();

		if( targetONode != nullptr )
		{
			unsigned short keyInsertedIndex = 0;
			std::shared_ptr<optr> oKey; std::shared_ptr<unsigned char> uKey = std::shared_ptr<unsigned char>( new unsigned char[20] );
			for( int i=0; i<_itemSet->keyCount(); i++ )
			{
				oKey = _itemSet->key(i); omemcpy( uKey.get(), oKey , KEY_SIZE );
				if( memcmp( uKey.get(), targetKey.get(), KEY_SIZE ) == 0 ){
					keyInsertedIndex = i;
					std::cout << "\x1b[31m" << "^---^---^---^-- >> " << keyInsertedIndex << "\x1b[m"	<< "\n";
					break;
				}
			}

			std::cout << "\x1b[31m" << "MARK" << "\x1b[m"	 << "\n";
			_itemSet->moveInsert( keyInsertedIndex + 1  , targetONode );
			_itemSet->childCount( _itemSet->keyCount() + 1 );
		}

		
		return nullptr; // 一応
	}

	// ノードが分割されることはないので単純に追加する

	return nullptr;
}



std::shared_ptr<ONode> ONode::child( unsigned short index )
{
	std::shared_ptr<ONode> ret = _itemSet->child( index );
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



std::shared_ptr<optr> ONode::subtreeFind( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<ONode> candidateChild;
	

	std::shared_ptr<optr> keyOptr;
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[5] );
	int flag;

	std::cout << _itemSet->keyCount() << "\n";
	std::cout << "(address) : ";
	_itemSet->Optr()->printAddr(); std::cout << "\n";
	for( int i=0; i<_itemSet->keyCount(); i++ )
	{
		sleep(1);
		keyOptr = _itemSet->key(i); omemcpy( rawKey.get(), keyOptr , 5 );

		std::cout << "---------------------------------------" << "\n";
		for( int i=0; i<20; i++ ){
			printf("%02X", targetKey.get()[i]);
		} std::cout << "\n";
		keyOptr->printValueContinuously(20); std::cout << "\n";
		std::cout << "---------------------------------------" << "\n";


		flag = memcmp( targetKey.get() , rawKey.get() , 5 );

		std::cout << "flag -> " << flag << "\n";

		if( flag < 0 ){
			std::cout << " << left" << "\n";
			candidateChild = _itemSet->child(i);
			if( candidateChild == nullptr ) return nullptr;
			goto direct;
		}
		else if( flag == 0 ){
			std::cout << "Hit!! with -> ";
			_itemSet->Optr()->printAddr(); std::cout << "\n";
			return nullptr;
			//return this->_itemSet->dataOptr(i);	
		}
		std::cout << "skiped" << "\n";
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
		unsigned char emptyOAddr[5]; memset( emptyOAddr, 0x00 , sizeof(emptyOAddr) );
		//std::shared_ptr<optr>	baseOptr = oMemoryManager->get( emptyOAddr );

		_rootONode = std::shared_ptr<ONode>( new ONode( oMemoryManager ) );
	}
}




void OBtree::rootONode( std::shared_ptr<ONode> target )
{
	_rootONode = target;
}




const std::shared_ptr<ONode> OBtree::rootONode()
{
	return _rootONode;
}




void OBtree::add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<ONode> targetONode )
{
	std::shared_ptr<ONode> deepestONode;

	std::shared_ptr<ONode> currentONode = _rootONode;

	std::cout << "@@@@@@@@@@@@@ " << _rootONode->isLeaf() << "\n";
	while( (currentONode->itemSet()->childCount() >= 1 ) )
	{
		std::shared_ptr<optr> keyOptr;
		std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		std::cout << "分岐初め" << "\n";
		for( int i=0; i<currentONode->itemSet()->childCount(); i++ )
		{
			keyOptr = currentONode->itemSet()->key(i);
			omemcpy( rawKey.get() , keyOptr , KEY_SIZE );

			if( memcmp( targetKey.get(), rawKey.get() , KEY_SIZE ) < 0 ){
				std::cout << "...................." << "\n";
				for( int j=0; j<20; j++){
					printf("%c", targetKey.get()[i]);
				} std::cout << "\n";
				for( int j=0; j<20; j++ ){
					printf("%c", rawKey.get()[i]);
				} std::cout << "\n";

				std::cout << "...................." << "\n";
				currentONode = currentONode->child(i);
				std::cout << "分岐ここ 0"	<< "\n";
				goto direct;
			}
		}

		currentONode = currentONode->child( currentONode->itemSet()->childCount()-1 );
		std::cout << "分岐ここ 2" << "\n";

		direct:
			void;
	}
	std::cout << "分岐終了" << "\n";

	deepestONode = currentONode;

	std::cout << "(DeepestNodeAddr) :: "; deepestONode->itemSet()->Optr()->printAddr(); std::cout << "\n";

	std::shared_ptr<ONode> newRootNode;
	newRootNode = deepestONode->recursiveAdd( targetKey , targetONode );
	if( newRootNode != nullptr )
		_rootONode = newRootNode;
	return;
}



std::shared_ptr<optr> OBtree::find( std::shared_ptr<unsigned char> targetKey )
{
	return _rootONode->subtreeFind( targetKey );
}



int OBtree::printSubTree( std::shared_ptr<ONode> subtreeRoot )
{

	std::cout << "\n\n============================================" << "\n";
	std::cout << "[ "; subtreeRoot->itemSet()->Optr()->printAddr(); std::cout << " ]\n";
	std::cout << "[ 親ノードアドレス ] :: ";
	subtreeRoot->parent()->itemSet()->Optr()->printAddr(); std::cout << "\n";

	std::cout << "[ キー個数 ] :: " << subtreeRoot->itemSet()->keyCount() << "\n";
	for( int i=0; i<subtreeRoot->itemSet()->keyCount(); i++ )
	{
		std::shared_ptr<optr> oKey = subtreeRoot->itemSet()->key(i);
		std::cout << "(" << i << ") "; oKey->printValueContinuously(20); std::cout << "\n";
	}
	std::cout << "[ 子ノード個数 ] :: " << subtreeRoot->itemSet()->childCount() << "\n";
	for( int i=0; i<subtreeRoot->itemSet()->childCount(); i++ )
	{
		std::shared_ptr<ONode> cON = subtreeRoot->itemSet()->child(i);
		std::cout << "(" << i << ")" ;
		cON->itemSet()->Optr()->printAddr(); std::cout << "\n";
	}
	std::cout << "============================================\n\n" << "\n";



	for( int i=0; i<subtreeRoot->itemSet()->childCount(); i++ )
	{
		OBtree::printSubTree( subtreeRoot->child(i) );
	}

	return 0;
}







} // close miya_db namespace
