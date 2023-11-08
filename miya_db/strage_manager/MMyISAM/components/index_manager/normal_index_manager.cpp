#include "normal_index_manager.h"

#include "./btree.h"


namespace miya_db{




NormalIndexManager::NormalIndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_masterBtree = 	std::shared_ptr<OBtree>( new OBtree(oMemoryManager) );
};




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




}; // close miya_db namespace
