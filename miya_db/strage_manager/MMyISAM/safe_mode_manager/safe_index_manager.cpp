#include "safe_index_manager.h"


#include "./safe_o_node.h"
#include "./safe_btree.h"
#include "../components/page_table/overlay_memory_manager.h"
#include "../components/index_manager/btree.h"
#include "../components/index_manager/o_node.h"
#include "../components/page_table/optr_utils.h"


namespace miya_db
{




SafeIndexManager::SafeIndexManager( std::string indexFilePath ,const std::shared_ptr<OBtree> normalOBtree , std::shared_ptr<unsigned char> migrationDBState ) : IndexManager( indexFilePath )
{
    std::cout << "THIS IS SAFE INDEX MANAGER CONSTRUCTOR" << "\n";
	_migrationDBState = migrationDBState;

    _conversionTable = std::shared_ptr<ONodeConversionTable>( new ONodeConversionTable(_oMemoryManager) );
    _conversionTable->safeOMemoryManager(_oMemoryManager); // 変換テーブルにSafe用のOverlayMemoryManagerをセットする
		_conversionTable->normalOMemoryManager( normalOBtree->overlayMemoryManager() );

    // ノーマルモードのOBtreeからSafeModeの基準となるOBtreeを作成する
	_masterBtree = std::shared_ptr<SafeOBtree>( new SafeOBtree( _conversionTable , normalOBtree->rootONode() ) );
	_conversionTable->init(); // entryMapを削除する

    _oMemoryManager->clear(); // セーフファイルはセーフモードが終了するまでの一時的なファイルなので都度削除する
	_oMemoryManager->allocate( 100 );

	/* 雑だがMeta領域をコピーしておく */
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	std::shared_ptr<optr> normalMetaOptr = std::make_shared<optr>( addrZero, _conversionTable->normalOMemoryManager()->dataCacheTable() );
	std::shared_ptr<optr> safeMetaOptr = std::make_shared<optr>( addrZero , _conversionTable->safeOMemoryManager()->dataCacheTable() );
	omemcpy( safeMetaOptr.get() , normalMetaOptr.get() , 100 );

	_oMemoryManager->allocate( SAFE_MODE_COLLICION_OFFSET );

	std::cout << "Safe Index Constructor Done" << "\n";

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

void SafeIndexManager::migrationDBState( std::shared_ptr<unsigned char> target )
{
  _migrationDBState = target;
}

std::shared_ptr<unsigned char> SafeIndexManager::migrationDBState()
{
  return _migrationDBState;
}


void SafeIndexManager::add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr )
{
  std::cout << "SafeInDexManager::add() called"  << "\n";
  _masterBtree->rootONode()->hello();
  _masterBtree->add( key, dataOptr );

  _oMemoryManager->syncDBState(); // 更新後状態を保存する
}


void SafeIndexManager::remove( std::shared_ptr<unsigned char> key )
{
	_masterBtree->remove( key );
	_oMemoryManager->syncDBState(); // 更新後状態を保存する
}


std::pair< std::shared_ptr<optr> ,int> SafeIndexManager::find( std::shared_ptr<unsigned char> key )
{
	std::shared_ptr< DataOptrEx > ret = _masterBtree->find( key );
	return std::make_pair( ret->first , ret->second );
}


std::shared_ptr<ONode> SafeIndexManager::mergeSafeBtree()
{
  std::shared_ptr<ONode> ret = _masterBtree->mergeSafeBtree( _masterBtree->SafeOBtree::rootONode() );
  _oMemoryManager->syncDBState(); // DB内部上他の書き込み/保存
  return ret;
}


void SafeIndexManager::printIndexTree()
{
    SafeOBtree::printSubTree( _masterBtree->rootONode() );
}






};
