#include "normal_index_manager.h"

#include "./btree.h"
#include "./o_node.h"


namespace miya_db{




NormalIndexManager::NormalIndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager ) : IndexManager( oMemoryManager )
{
	// _oMemoryManager = oMemoryManager;
	_masterBtree = 	std::shared_ptr<OBtree>( new OBtree(_oMemoryManager) );
	_oMemoryManager->syncDBState();
};


NormalIndexManager::NormalIndexManager( std::string indexFilePath ) : IndexManager( indexFilePath )
{
	// _oMemoryManager	 = std::make_shared<OverlayMemoryManager>( indexFilePath );
	_masterBtree = std::make_shared<OBtree>( _oMemoryManager );
	_oMemoryManager->syncDBState();
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


std::pair< std::shared_ptr<optr>, int > NormalIndexManager::find( std::shared_ptr<unsigned char> key )
{
	return std::make_pair( _masterBtree->find( key ) , DATA_OPTR_LOCATED_AT_NORMAL ); // あとで修正する 0: Normal
};


void NormalIndexManager::printIndexTree()
{
	OBtree::printSubTree( _masterBtree->rootONode() );
}



}; // close miya_db namespace
