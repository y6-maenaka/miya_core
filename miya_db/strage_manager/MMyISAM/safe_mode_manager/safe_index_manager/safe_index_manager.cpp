#include "safe_index_manager.h"

#include "./safe_o_node.h"
#include "./safe_btree.h"
#include "../../components/page_table/overlay_memory_manager.h"


namespace miya_db
{


ONodeConversionTable SafeONode::_conversionTable = ONodeConversionTable( nullptr );



SafeIndexManager::SafeIndexManager( std::shared_ptr<ONode> normalRootONode )
{
    std::string systemSafeDirectories = "../miya_db/table_fiels/.system/safe"; // あとでグローバルに静的に定義する
    // OverlayMemoryManagerの作成
    std::shared_ptr<OverlayMemoryManager> safeOverlayMemoryManager = std::make_shared<OverlayMemoryManager>( systemSafeDirectories );

   SafeIndexManager::_masterBtree = std::make_shared<SafeOBtree>( normalRootONode );
}



void SafeIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
    return;
}





void SafeIndexManager::remove( std::shared_ptr<unsigned char> key )
{
    return;
}


std::shared_ptr<optr> SafeIndexManager::find( std::shared_ptr<unsigned char> key )
{
    return nullptr;
}



};