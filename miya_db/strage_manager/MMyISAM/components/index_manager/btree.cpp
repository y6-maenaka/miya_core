#include "btree.h"



namespace miya_db
{





std::unique_ptr<optr> ONodeItemSet::key( unsigned short index )
{
	return ( (*_optr) + (index * INDEX_KEY_SIZE));
}




OBtreeNode* ONodeItemSet::child( unsigned short index )
{
	return nullptr;
}


std::unique_ptr<optr> ONodeItemSet::dataPtr( unsigned short index )
{
	return ( (*_optr) + ((index * 5) + (INDEX_KEY_SIZE * DEFAULT_THRESHOLD) + (5 * DEFAULT_THRESHOLD + 1)) );	
}




void OBtreeNode::registIndex( unsigned char* key , optr *dataPtr , optr* leftChildNode , optr* rightChildNode )
{

}



OBtreeNode* OBtreeNode::find( unsigned char *key )
{
	for(int i=0; i<DEFAULT_THRESHOLD; i++)
	{
		_oNodeItemSet->child(i);
	}

}



OBtree::OBtree()
{
	if( _rootNode == nullptr ) _rootNode = new OBtreeNode;
}



const OBtreeNode *OBtree::rootNode()
{
	return _rootNode;
}



}; // close miya_db namespace
