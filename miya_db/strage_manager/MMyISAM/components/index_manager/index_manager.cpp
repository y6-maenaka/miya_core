#include "index_manager.h"

#include "../page_table/overlay_memory_manager.h"


namespace miya_db
{


IndexManager::IndexManager( std::string filePath )
{
  _oMemoryManager = std::make_shared<OverlayMemoryManager>( filePath );
}

void IndexManager::clear()
{
  _oMemoryManager->clear();
}

IndexManager::IndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
  _oMemoryManager = oMemoryManager;
}


std::shared_ptr<OverlayMemoryManager> IndexManager::oMemoryManager()
{
  return _oMemoryManager;
}





}
