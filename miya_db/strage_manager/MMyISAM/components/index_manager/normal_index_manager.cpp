#include "normal_index_manager.h"

#include "./btree.h"
#include "./o_node.h"


namespace miya_db{




NormalIndexManager::NormalIndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;
	_masterBtree = 	std::shared_ptr<OBtree>( new OBtree(oMemoryManager) );
};


NormalIndexManager::NormalIndexManager( std::string indexFilePath )
{
	_oMemoryManager	 = std::make_shared<OverlayMemoryManager>( indexFilePath );
	_masterBtree = std::make_shared<OBtree>( _oMemoryManager );

	printf("NormalIndexManager :: OverlayMemoryManager Initialized with %p\n", _oMemoryManager.get() );
}




const std::shared_ptr<OBtree> NormalIndexManager::masterBtree()
{
	return _masterBtree;
}

void NormalIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
	return _masterBtree->add( key , dataOptr );
};



void NormalIndexManager::remove( std::shared_ptr<unsigned char> key )
{
	return _masterBtree->remove( key );
}




std::shared_ptr<optr> NormalIndexManager::find( std::shared_ptr<unsigned char> key )
{
	return _masterBtree->find( key );
};





void NormalIndexManager::printIndexTree()
{
	OBtree::printSubTree( _masterBtree->rootONode() );
}



}; // close miya_db namespace
