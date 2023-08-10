#include "btree.h"
#include "../page_table/optr_utils.h"


namespace miya_db
{






void ViewItemSet::importItemSet( ONodeItemSet *itemSet )
{
	std::array<unsigned char*, DEFAULT_KEY_COUNT> *keySorce = itemSet->exportKeyArray();
	std::copy( keySorce->begin() ,keySorce->end() , _key->begin() ); // 上書きで大丈夫

	std::array<ONode*, DEFAULT_CHILD_COUNT> *childSorce = itemSet->exportChildArray();
	std::copy( childSorce->begin() ,childSorce->end(), _child->begin() ); // 上書き

	return;
}















/*
  ---------------- キー操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::keyCount()
{
	return static_cast<unsigned short>( (*_optr + NODE_OPTR_SIZE/*親ノードへのポインタ分のサイズ*/)->value() ); // itemSet先頭1バイトがそのままkeyCountになる
}

std::unique_ptr<optr> ONodeItemSet::key( unsigned short index ) // index : start with 0
{
	if( index+1  > (keyCount()) ) return nullptr;

	return *_optr + ( NODE_OPTR_SIZE + ELEMENT_COUNT_SIZE + (index * KEY_SIZE));
}

void ONodeItemSet::key( unsigned short index , unsigned char* targetKey )
{
	if( index >= DEFAULT_KEY_COUNT ) return;

	omemcpy( (*_optr + ( NODE_OPTR_SIZE + ELEMENT_COUNT_SIZE + (index * KEY_SIZE))).get() , targetKey , KEY_SIZE );
}


void ONodeItemSet::keyCount( unsigned short num )
{
	(*_optr + NODE_OPTR_SIZE)->value( static_cast<unsigned char>(num) );
}
/* -------------------------------------------------------------- */




/*
	---------------- 子ノード操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::childCount()
{
	return static_cast<unsigned short>( (*_optr + ( NODE_OPTR_SIZE + ELEMENT_COUNT_SIZE/*keycountのサイズ*/ + (DEFAULT_THRESHOLD*KEY_SIZE)))->value() );
}

void ONodeItemSet::childCount( unsigned short num )
{	
	(*_optr + ( NODE_OPTR_SIZE + ELEMENT_COUNT_SIZE/*keycountのサイズ*/ + (DEFAULT_THRESHOLD*KEY_SIZE)))->value( static_cast<unsigned char>(num)) ;
}

std::unique_ptr<ONode> ONodeItemSet::child( unsigned short index )
{
	// 指定インデックスのoptrを取得する
	if( index+1 > (childCount()) ) return nullptr;
				
	std::unique_ptr<optr> childHead = *_optr + ( NODE_OPTR_SIZE/*親ノードへのポインタサイズ*/ + ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*KEY_SIZE) + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE));
	std::unique_ptr<ONodeItemSet> itemSet = std::make_unique<ONodeItemSet>( ONodeItemSet(childHead.get()) );

	return std::make_unique<ONode>( ONode(itemSet.get() ));	
}
/* -------------------------------------------------------------- */




/*
	---------------- データオーバレイポインタ操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::dataOptrCount()
{
	return static_cast<unsigned short>( (*_optr + (( NODE_OPTR_SIZE/*親ノードへのポインタサイズ*/ + ELEMENT_COUNT_SIZE/*keycountのサイズ*/ + (DEFAULT_THRESHOLD*KEY_SIZE)) + ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*NODE_OPTR_SIZE)))->value() );
}

std::unique_ptr<optr> ONodeItemSet::dataOptr( unsigned short index )
{
	if( index+1 > (dataOptrCount()) ) return nullptr;
	
	return *_optr + ( NODE_OPTR_SIZE/*親ノードへのポインタサイズ*/ + ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*KEY_SIZE) +ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*NODE_OPTR_SIZE) + ELEMENT_COUNT_SIZE + (index*DATA_OPTR_SIZE));
}

/* -------------------------------------------------------------- */






std::array<optr*, DEFAULT_KEY_COUNT> *ONodeItemSet::exportKeyOptrArray()
{
	std::array<optr*, DEFAULT_KEY_COUNT> *ret = new std::array<optr*, DEFAULT_KEY_COUNT>;

	for( int i=0; i<keyCount(); i++ )
		ret->at(i) = key(i).get();

	return ret;
}



std::array<unsigned char*, DEFAULT_KEY_COUNT> *ONodeItemSet::exportKeyArray()
{
	std::array<unsigned char*, DEFAULT_KEY_COUNT> *ret = new std::array<unsigned char* , DEFAULT_KEY_COUNT>;

	for( int i=0; i<keyCount(); i++)
		ret->at(i) = key(i)->mapToMemory( KEY_SIZE ).get();	

	return ret;
}



std::array<ONode*, DEFAULT_CHILD_COUNT> *ONodeItemSet::exportChildArray()
{
	std::array<ONode*, DEFAULT_CHILD_COUNT> *ret = new std::array<ONode*, DEFAULT_CHILD_COUNT>;

	for( int i=0; i < childCount(); i++ )
		ret->at(i) = child(i).get();
	
	return ret;
}





















ONode::ONode( optr* optr )
{
	_itemSet = new ONodeItemSet( optr );
}


ONode::ONode( ONodeItemSet *itemSet )
{
	_itemSet = itemSet;
	if( itemSet->childCount() == 0 ) _isLeaf = true;
}



std::unique_ptr<ONode> ONode::parent()
{
	return std::make_unique<ONode>( ONode(_itemSet->Optr()) );
}


void ONode::parent( ONode* targetONode )
{
	omemcpy( _itemSet->Optr() , targetONode->itemSet()->Optr() , NODE_OPTR_SIZE );
	return;
}


void ONode::add( unsigned char* targetKey, ONode* targetONode )
{

	if( _itemSet->keyCount() >= DEFAULT_THRESHOLD-1 ) // ノードの分割が発生するパターン
	{ 

		auto splitONode = [&]( unsigned char* _targetKey , ONode* _targetONode ) // targetKeyとtargetONodeは入出力引数となる
		{

			ViewItemSet viewItemSet;
			viewItemSet.importItemSet( _itemSet ); // 仮想アイテムセットに既存のアイテムセット情報を取り込む

			// 一旦仮想アイテムセットに対象要素を追加する
			viewItemSet._key->at( DEFAULT_KEY_COUNT ) = _targetKey;

			if( _targetONode != nullptr ) // 追加対象がリーフノードでない限りは追加される
				viewItemSet._child->at( DEFAULT_CHILD_COUNT ) = _targetONode;


			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;
			unsigned char* separatorKey = viewItemSet._key->at( separatorKeyIndex ); // これは親ノードへの追加対象となる


			// 論理アイテムセットのキーをソートする
			std::sort( viewItemSet._key->begin() , viewItemSet._key->end() , [](unsigned char* a, unsigned char *b){
				return memcmp( a , b , KEY_SIZE );
			});	

			// 分割ルーチンスタート 
			// このアイテムセットは右子ノードとなる　分割対象の中央右ノードを格納する
			ONode* splitONode = new ONode; // (新たな右子ノード)  親ノードへの挿入対象となる
			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	
				splitONode->itemSet()->key( i , viewItemSet._key->at(i+separatorKeyIndex) );
			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			splitONode->parent( this->parent().get() );


			// このアイテムセットは左子ノードとなる 分割対象の中央左のキーを格納する
			for( int i=0; i<separatorKeyIndex; i++)
				_itemSet->key( i , viewItemSet._key->at(i) );
			_itemSet->keyCount(separatorKeyIndex);

			_targetKey = separatorKey;
			_targetONode = splitONode;

			//return parent()->add( targetKey , targetONode );
		};

		splitONode( targetKey , targetONode );

		std::unique_ptr<ONode> parentONode = parent();
		if(( parentONode.get() , 0x0000000000 , NODE_OPTR_SIZE ) == 0)  // 分裂先がルートノード場合
		{
			ONode* newRootNode = new ONode;
			return ;
		}


		return parentONode->add( targetKey, targetONode );
	}

	else{ // 単純追加
		_itemSet->key( _itemSet->keyCount() , targetKey );
	}

	// ノードが分割されることはないので単純に追加する

	return;
}



void ONode::regist( unsigned char *targetKey , optr *targetDataOptr )
{

	std::unique_ptr<ONode> insertTargetONode = subtreeKeySearch( this , targetKey );
	

}






std::unique_ptr<ONode> ONode::subtreeKeySearch( ONode* targetONode ,unsigned char *targetKey )
{
	if( targetONode->isLeaf() ) return std::unique_ptr<ONode>(targetONode);

	int keyCount = targetONode->oNodeItemSet()->keyCount();
	int i=0; int course;

	for(i=0; i<keyCount; i++)
	{
		course = ocmp( targetKey ,targetONode->oNodeItemSet()->key(i).get() , KEY_SIZE );
		if( course <= 0 ) break;
	}


	if( course == 0 ) return std::unique_ptr<ONode>( targetONode );
	if( course < 0 ) return subtreeKeySearch( targetONode->oNodeItemSet()->child(i).get() , targetKey ); 

	return subtreeKeySearch( targetONode->oNodeItemSet()->child(keyCount-1).get() , targetKey );
}


















OBtree::OBtree( ONode *oNode )
{
	//if( oNode == nullptr ) _rootONode = new ONode;
	//_rootONode = oNode;
}



const ONode *OBtree::rootNode()
{
	return _rootONode;
}















} // close miya_db namespace
