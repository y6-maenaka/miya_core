#include "safe_mode_manager.h"


#include "../components/value_store_manager/value_store_manager.h"
#include "../components/index_manager/index_manager.h"



namespace miya_db
{





MMyISAMSafeModeManager::MMyISAMSafeModeManager( std::shared_ptr<ValueStoreManager> normalValueStoreManager, std::shared_ptr<IndexManager> normalIndexManager )
{
	_normalModules._valueStoreManager = normalValueStoreManager;
	_normalModules._indexManager = normalIndexManager;
}










};
