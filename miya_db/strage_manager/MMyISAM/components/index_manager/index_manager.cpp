#include "index_manager.h"

#include "./btree.h"


namespace miya_db{




IndexManager::IndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_masterBtree = 	std::shared_ptr<OBtree>( new OBtree(oMemoryManager) );

	
};




void IndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
	return _masterBtree->add( key , dataOptr );
};




std::shared_ptr<optr> IndexManager::find( std::shared_ptr<unsigned char> key )
{
	return _masterBtree->find( key );
};




}; // close miya_db namespace
