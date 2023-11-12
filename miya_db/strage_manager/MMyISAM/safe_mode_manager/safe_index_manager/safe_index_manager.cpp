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
    
    SafeONode::_conversionTable.safeOMemoryManager( safeOverlayMemoryManager ); // 変換テーブルにSafe用のOverlayMemoryManagerをセットする
	printf("ConversionTable::SafeOverlayMemoryManager initialized with %p\n" , SafeONode::_conversionTable.safeOMemoryManager().get() );
	SafeONode::_conversionTable.normalOMemoryManager( normalOBtree->overlayMemoryManager() );
	printf("ConversionTable::NormalOverlayMemoryManager initialized with %p\n" , SafeONode::_conversionTable.normalOMemoryManager().get() );

    _masterBtree = std::make_shared<SafeOBtree>( normalOBtree->rootONode() );
    SafeONode::_conversionTable.init(); // entryMapを削除する

	printf("%p\n", _masterBtree->rootONode()->citemSet().get() );
    safeOverlayMemoryManager->clear(); // セーフファイルはセーフモードが終了するまでの一時的なファイルなので都度削除する
		safeOverlayMemoryManager->allocate( 100 );
		safeOverlayMemoryManager->allocate( SAFE_MODE_COLLICION_OFFSET ); 
    /* 重要な操作 ※この操作をしないと,ConversionTableでNormalIndexかSafeIndexのアドレスが重複した場合変換ループが発生してしまう */
}



void SafeIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
    std::cout << "add() with SafeIndexManager" << "\n";
    _masterBtree->rootONode()->hello();
    return _masterBtree->add( key, dataOptr );
}



void SafeIndexManager::remove( std::shared_ptr<unsigned char> key )
{
	std::cout << "remove() with SafeIndexManager" << "\n";
	return _masterBtree->remove( key );
}


std::shared_ptr<optr> SafeIndexManager::find( std::shared_ptr<unsigned char> key )
{
    return nullptr;
}


void SafeIndexManager::printIndexTree()
{
    SafeOBtree::printSubTree( _masterBtree->rootONode() );
}


};
