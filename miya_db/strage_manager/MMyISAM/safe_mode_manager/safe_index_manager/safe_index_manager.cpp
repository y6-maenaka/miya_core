#include "safe_index_manager.h"


#include "./safe_o_node.h"
#include "./safe_btree.h"
#include "../../components/page_table/overlay_memory_manager.h"
#include "../../components/index_manager/btree.h"
#include "../../components/index_manager/o_node.h"


namespace miya_db
{


ONodeConversionTable SafeONode::_conversionTable = ONodeConversionTable( nullptr );



SafeIndexManager::SafeIndexManager( const std::shared_ptr<OBtree> normalOBtree )
{
    std::cout << "THIS IS SAFE INDEX MANAGER CONSTRUCTOR" << "\n";

    std::string systemSafeDirectories = "../miya_db/table_files/.system/safe/safe_index"; // あとでグローバルに静的に定義する
    // OverlayMemoryManagerの作成
    std::shared_ptr<OverlayMemoryManager> safeOverlayMemoryManager = std::make_shared<OverlayMemoryManager>( systemSafeDirectories );
    _masterBtree = std::make_shared<SafeOBtree>( normalOBtree->rootONode() );

    safeOverlayMemoryManager->clear(); // セーフファイルはセーフモードが終了するまでの一時的なファイルなので都度削除する
    safeOverlayMemoryManager->allocate( O_NODE_ITEMSET_SIZE / 2 ); 
    /* 重要な操作 ※この操作をしないと,ConversionTableでNormalIndexかSafeIndexのアドレスが重複した場合変換ループが発生してしまう */
}



void SafeIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
    std::cout << "add() with SafeIndexManager" << "\n";
    _masterBtree->rootONode()->hello();
    _masterBtree->rootONode()->itemSet();
    return _masterBtree->add( key, dataOptr );
}





void SafeIndexManager::remove( std::shared_ptr<unsigned char> key )
{
    return;
}


std::shared_ptr<optr> SafeIndexManager::find( std::shared_ptr<unsigned char> key )
{
    return nullptr;
}


void SafeIndexManager::printIndexTree()
{
    OBtree::printSubTree( _masterBtree->rootONode() );
}


};