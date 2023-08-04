#include "btree.h"



namespace miya_db
{



/*
  ---------------- キー操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::keyCount()
{
	return static_cast<unsigned short>( _optr->value() ); // itemSet先頭1バイトがそのままkeyCountになる
}

std::unique_ptr<optr> ONodeItemSet::key( unsigned short index ) // index : start with 0
{
	if( index+1  > (keyCount()) ) return nullptr;

	return *_optr + (ELEMENT_COUNT_SIZE + (index * KEY_SIZE));
}
/* -------------------------------------------------------------- */




/*
	---------------- 子ノード操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::childCount()
{
	return static_cast<unsigned short>( (*_optr + ( ELEMENT_COUNT_SIZE/*keycountのサイズ*/ + (DEFAULT_THRESHOLD*KEY_SIZE)))->value() );
}

std::unique_ptr<ONodeItemSet> ONodeItemSet::child( unsigned short index )
{
	// 指定インデックスのoptrを取得する
	if( index+1 > (childCount()) ) return nullptr;
				
	std::unique_ptr<optr> childHead = *_optr + ( ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*KEY_SIZE) + ELEMENT_COUNT_SIZE + (index*CHILD_OPTR_SIZE));

	return std::make_unique<ONodeItemSet>( ONodeItemSet(childHead.get()) );
}
/* -------------------------------------------------------------- */




/*
	---------------- データオーバレイポインタ操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::dataOptrCount()
{
	return static_cast<unsigned short>( (*_optr + (( ELEMENT_COUNT_SIZE/*keycountのサイズ*/ + (DEFAULT_THRESHOLD*KEY_SIZE)) + ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*CHILD_OPTR_SIZE)))->value() );
}

std::unique_ptr<optr> ONodeItemSet::dataOptr( unsigned short index )
{
	if( index+1 > (dataOptrCount()) ) return nullptr;
	
	return *_optr + ( ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*KEY_SIZE) +ELEMENT_COUNT_SIZE + (DEFAULT_THRESHOLD*CHILD_OPTR_SIZE) + ELEMENT_COUNT_SIZE + (index*DATA_OPTR_SIZE));
}

/* -------------------------------------------------------------- */









void OBtreeNode::registIndex( unsigned char* key , optr *dataPtr , optr* leftChildNode , optr* rightChildNode )
{

}



OBtreeNode* OBtreeNode::subtreeKeySearch( unsigned char *key )
{
	return nullptr;
}



OBtree::OBtree()
{
	if( _rootNode == nullptr ) _rootNode = new OBtreeNode;
}



const OBtreeNode *OBtree::rootNode()
{
	return _rootNode;
}


} // close miya_db namespace
