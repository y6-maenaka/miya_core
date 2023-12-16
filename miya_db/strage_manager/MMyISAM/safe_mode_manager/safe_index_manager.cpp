#include "safe_index_manager.h"


#include "./safe_o_node.h"
#include "./safe_btree.h"
#include "../components/page_table/overlay_memory_manager.h"
#include "../components/index_manager/btree.h"
#include "../components/index_manager/o_node.h"
#include "../components/page_table/optr_utils.h"


namespace miya_db
{




SafeIndexManager::SafeIndexManager( std::string indexFilePath ,const std::shared_ptr<OBtree> normalOBtree )
{
    std::cout << "THIS IS SAFE INDEX MANAGER CONSTRUCTOR" << "\n";

    // std::string systemSafeDirectories = "../miya_db/table_files/.system/safe/safe_index"; // あとでグローバルに静的に定義する
    // OverlayMemoryManagerの作成
    std::shared_ptr<OverlayMemoryManager> safeOverlayMemoryManager = std::make_shared<OverlayMemoryManager>( indexFilePath );

    _conversionTable = std::shared_ptr<ONodeConversionTable>( new ONodeConversionTable(safeOverlayMemoryManager) );
    _conversionTable->safeOMemoryManager( safeOverlayMemoryManager ); // 変換テーブルにSafe用のOverlayMemoryManagerをセットする
		_conversionTable->normalOMemoryManager( normalOBtree->overlayMemoryManager() );

    // ノーマルモードのOBtreeからSafeModeの基準となるOBtreeを作成する
    _masterBtree = std::make_shared<SafeOBtree>(  _conversionTable ,normalOBtree->rootONode() ); // rootONodeが存在しない場合は
    _conversionTable->init(); // entryMapを削除する

    safeOverlayMemoryManager->clear(); // セーフファイルはセーフモードが終了するまでの一時的なファイルなので都度削除する
		safeOverlayMemoryManager->allocate( 100 );

		/* 雑だがMeta領域をコピーしておく */
		unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
		std::shared_ptr<optr> normalMetaOptr = std::make_shared<optr>( addrZero, _conversionTable->normalOMemoryManager()->dataCacheTable() );
		std::shared_ptr<optr> safeMetaOptr = std::make_shared<optr>( addrZero , _conversionTable->safeOMemoryManager()->dataCacheTable() );
		omemcpy( safeMetaOptr.get() , normalMetaOptr.get() , 100 );


		safeOverlayMemoryManager->allocate( SAFE_MODE_COLLICION_OFFSET );

    /* 重要な操作 ※この操作をしないと,ConversionTableでNormalIndexかSafeIndexのアドレスが重複した場合変換ループが発生してしまう */
}



const std::shared_ptr<SafeOBtree> SafeIndexManager::masterBtree()
{
    return _masterBtree;
}

std::shared_ptr<ONodeConversionTable> SafeIndexManager::conversionTable()
{
  return _conversionTable;
}

void SafeIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
  std::cout << "SafeInDexManager::add() called"  << "\n";
  _masterBtree->rootONode()->hello();
  return _masterBtree->add( key, dataOptr );
}



void SafeIndexManager::remove( std::shared_ptr<unsigned char> key )
{
	return _masterBtree->remove( key );
}


std::pair< std::shared_ptr<optr> ,int> SafeIndexManager::find( std::shared_ptr<unsigned char> key )
{
	std::shared_ptr< DataOptrEx > ret = _masterBtree->find( key );
	return std::make_pair( ret->first , ret->second );
}



std::shared_ptr<ONode> SafeIndexManager::mergeSafeBtree()
{
  return _masterBtree->mergeSafeBtree( _masterBtree->SafeOBtree::rootONode() );
}


void SafeIndexManager::printIndexTree()
{
    SafeOBtree::printSubTree( _masterBtree->rootONode() );
}


};
