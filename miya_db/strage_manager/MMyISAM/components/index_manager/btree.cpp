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
			

	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << "\n";
	_optr->printAddr(); std::cout << "\n";
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << "\n";

	std::shared_ptr<optr> childHead = *_optr + ( CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE) );
	//std::shared_ptr<optr> sChildHead = std::move( childHead ); // 仮で無理やり変換しておく

	//return std::make_shared<ONode>( new ONode( sChildHead ) );
	
	
	unsigned char oAddr[5];
	omemcpy( oAddr , childHead , 5 );

	std::shared_ptr<optr> childOptr = std::make_shared<optr>( oAddr, childHead->cacheTable() );
	
	return std::make_shared<ONode>(childOptr  );

	//return std::make_shared<optr>( oAddr , _optr->cacheTable() );
	


	//return std::make_shared<ONode>( childHead );
	//return nullptr;
}


void ONodeItemSet::child( unsigned short index , std::shared_ptr<ONode> targetONode )
{
	if( index > DEFAULT_CHILD_COUNT ) return;

	std::cout << "------------------" << "\n";
	std::cout << "child() ) setter " << "\n";
	targetONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
	std::cout << "------------------" << "\n";

	omemcpy( (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , targetONode->itemSet()->Optr()->addr() , NODE_OPTR_SIZE );

	std::cout << "=====================" << "\n";
	std::cout << "after" << "\n";
	targetONode->itemSet()->Optr()->printAddr(); std::cout << "\n";
	for( int i=0; i<5; i++ )
	{
		printf("%02X", (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE) +i ))->value() );
	} std::cout << "\n";
	std::cout << "=====================" << "\n";


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
	std::cout << ":::::::::::::::::::::::" << "\n";
	baseOptr->printAddr(); std::cout << "\n";
	baseOptr->printValueContinuously(5); std::cout << "\n";
	_itemSet->Optr()->printAddr(); std::cout << "\n";
	std::cout << ":::::::::::::::::::::::" << "\n";
}



ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;

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
	std::cout << "こーるど"	 << "\n";
	_itemSet->Optr()->printValueContinuously(5); std::cout << "\n";
	return std::make_shared<ONode>( _itemSet->parent() );
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





void ONode::add( std::shared_ptr<unsigned char> targetKey, std::shared_ptr<ONode> targetONode )
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
			viewItemSet._key.at( DEFAULT_KEY_COUNT ) = *_targetKey; 

			std::cout << "check 0" << "\n";

			if( *_targetONode != nullptr ) // 追加対象がリーフノードでない限りは追加される
				viewItemSet._child.at( DEFAULT_CHILD_COUNT ) = *_targetONode;

			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;

			std::cout << "check 1" << "\n";
				
			// 論理アイテムセットのキーをソートする
			std::sort( viewItemSet._key.begin() , viewItemSet._key.end() , []( std::shared_ptr<unsigned char> a, std::shared_ptr<unsigned char> b){
				return memcmp( a.get() , b.get() , KEY_SIZE ) <= 0;
			});	 // viewItemSetのメソッドとした方が良い


			std::cout << "check 3" << "\n";

			std::shared_ptr<unsigned char> separatorKey = viewItemSet._key.at( separatorKeyIndex ); // これは親ノードへの追加対象となる
			
			// 分割ルーチンスタート 
			// このアイテムセットは右子ノードとなる　分割対象の中央右ノードを格納する		
			std::shared_ptr<ONode> splitONode = std::make_shared<ONode>( _oMemoryManager ); // (新たな右子ノード)  親ノードへの挿入対象となる 子ノードとして作成する
			// メモリーマネージャーだけ渡すと	
		

			// エラーでそう
			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	{
				splitONode->itemSet()->key( i , viewItemSet._key.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			splitONode->parent( this->parent() );

			std::cout << ">>>>>>>  " << splitONode->itemSet()->key(0)->value() << "\n";

		
			// 本アイテムセットは左子ノードとなる 分割対象の中央左のキーを格納する
			for( int i=0; i<separatorKeyIndex; i++)
				_itemSet->key( i , viewItemSet._key.at(i) );
			_itemSet->keyCount(separatorKeyIndex);


			std::cout << "<<<<<<<<<<<<  " << _itemSet->key(0)->value(); std::cout << "\n";
			std::cout << "check 6" << "\n";

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
			return;  // 必ずリターンする
		}

		return parentONode->add( targetKey, targetONode );
	}

	else{ // 単純追加
		std::cout << "キーの単純追加"	 << "\n";
		for( int i=0; i<20; i++ )
		{
			printf("%c", targetKey.get()[i]);
		} std::cout << "\n";

		_itemSet->key( _itemSet->keyCount() , targetKey );
		_itemSet->keyCount( _itemSet->keyCount() + 1 );
		return; // 一応
	}

	// ノードが分割されることはないので単純に追加する

	return;
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




void ONode::regist( unsigned char *targetKey , optr *targetDataOptr )
{

	std::unique_ptr<ONode> insertTargetONode = subtreeKeySearch( this , targetKey );
	

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




const std::shared_ptr<ONode> OBtree::rootONode()
{
	return _rootONode;
}












} // close miya_db namespace
