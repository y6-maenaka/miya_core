#include "MMyISAM.h"

#include "./components/page_table/overlay_memory_manager.h"
#include "./components/page_table/overlay_memory_allocator.h"
#include "./components/page_table/overlay_ptr.h"
#include "./components/page_table/optr_utils.h"
#include "./components/page_table/cache_manager/cache_table.h"
#include "./components/page_table/cache_manager/mapper/mapper.h"

#include "./components/index_manager/index_manager.h"
#include "./components/index_manager/normal_index_manager.h"

#include "./components/value_store_manager/value_store_manager.h"

#include "../../miya_db/query_context/query_context.h"

#include "./safe_mode_manager/safe_index_manager.h"
#include "./safe_mode_manager/safe_o_node.h"
#include "./safe_mode_Manager/safe_btree.h"

namespace miya_db
{


MMyISAM::MMyISAM( std::string dbName )
{
	_dbName = dbName;

   // データストアの初期化
   std::string filePath = "../miya_db/table_files/" + dbName + "/" + dbName;
	_normal._valueStoreManager = std::make_shared<ValueStoreManager>( filePath );

    // インデックスの初期化
    filePath += "_index";
	_normal._indexManager = std::make_shared<NormalIndexManager>( filePath );

	// 初回起動時は通常モードで起動する
	//_normal._valueStoreManager = _normal._valueStoreManager.get();
	// _normal._indexManager =
}

void MMyISAM::clearSafeMode()
{
  std::fill( _safe._activeSafeIndexManagerArray.begin(), _safe._activeSafeIndexManagerArray.end(), nullptr);
  std::fill( _safe._activeValueStoreManagerArray.begin() , _safe._activeValueStoreManagerArray.end(), nullptr );
}


bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
	short registryIndex = qctx->registryIndex();
	if( registryIndex >= 0 ) // SafeMode // RegistryIndexが0以上 => SafeMode
	{
	  auto valueStoreManager = _safe._activeValueStoreManagerArray.at(registryIndex);
	  if( valueStoreManager == nullptr ) return false;

	  std::shared_ptr<optr> storeTarget = valueStoreManager->add( qctx );

	  auto indexManager = _safe._activeSafeIndexManagerArray.at(registryIndex);
	  if( indexManager == nullptr ) return false;
	  indexManager->add( qctx->key(), storeTarget );
	}
	else // NorMalMode // RegistryIndexが0未満 => NormalMode
	{
	  std::shared_ptr<optr> storeTarget = _normal._valueStoreManager->add( qctx );
	  _normal._indexManager->add( qctx->key(), storeTarget );

	  _normal._indexManager->oMemoryManager()->syncDBState(); // 内部状態変数の更新
	  clearSafeMode(); // 本(Normal)DBに変更があったので,SafeModeはすべてクローズする
	}

	std::cout << "\x1b[36m" << "(MiyaDB) add with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";

	return true;
}


bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
	short registryIndex = qctx->registryIndex();
	std::pair< std::shared_ptr<optr>, int > dataOptr;
	std::shared_ptr<ValueStoreManager> valueStoreManager;

	if( registryIndex >= 0 ) // セーフモード
	{
	  auto safeIndexManager = _safe._activeSafeIndexManagerArray.at(registryIndex);
	  if( safeIndexManager == nullptr ) return false;

	  dataOptr = safeIndexManager->find( qctx->key() );
	  valueStoreManager = _safe._activeValueStoreManagerArray.at(registryIndex);
	}
	else // ノーマル
	{
	  dataOptr = _normal._indexManager->find( qctx->key() );
	  valueStoreManager = _normal._valueStoreManager;
	}

	std::cout << "\x1b[32m" << "(MiyaDB) get with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";

	if( dataOptr.first == nullptr ) return false;
	size_t dataLength; std::shared_ptr<unsigned char> data;

	/* あまり良くないデータストアの切り替え方法 */
	if( dataOptr.second == 0 ) // SafeModeで0以外が返却されることは実装上ありあえないので大丈夫だと思う
		dataLength = valueStoreManager->get( dataOptr.first , &data );
	if( dataOptr.second == 1 ) // Safeに存在する場合
		dataLength = valueStoreManager->get( dataOptr.first , &data );

	qctx->value( data , dataLength );
	return true;
}


bool MMyISAM::remove( std::shared_ptr<QueryContext> qctx )
{
	std::cout << "\x1b[35m" << "(MiyaDB) remove with key :: ";

	short registryIndex = qctx->registryIndex();
	if( registryIndex >= 0 ) // SafeMode
	{
	  auto safeIndexManager = _safe._activeSafeIndexManagerArray.at(registryIndex);
	  if( safeIndexManager == nullptr ) return false;
	  safeIndexManager->remove( qctx->key() );
	}
	else // NormalMode
	{
	  _normal._indexManager->remove( qctx->key() );
	  clearSafeMode();
	}


	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i] );
	}  std::cout << "\x1b[39m"  << "\n";

	// _indexManager->remove( qctx->key() );
  // 本来はvalueStoreからも削除する

	return true;
}


bool MMyISAM::exists( std::shared_ptr<QueryContext> qctx )
{
	std::pair< std::shared_ptr<optr>, int > retDataOptr;
	short registryIndex = qctx->registryIndex();

	if( registryIndex >= 0 ) //Safeモード
	{
	  auto indexManager = _safe._activeSafeIndexManagerArray.at(registryIndex);
	  if( indexManager == nullptr ) return false;

	  indexManager->find( qctx->key() );
	}
	else // Normalモード
	{
	  _normal._indexManager->find( qctx->key() );
	}

	// 簡易的にキーが存在するか否かでデータの存在有無を判別することとする
	if( retDataOptr.first == nullptr ) return false; // キーが存在しない : false
	return true; // キーが存在しない : true
}


bool MMyISAM::migrateToSafeMode( std::shared_ptr<QueryContext> qctx )
{
	short registryIndex = qctx->registryIndex();
	std::cout << "SwitchToSafeMode IndexWith :: " << registryIndex << "\n";
	std::string safeDirPath = "../miya_db/table_files/.system/safe/" + _dbName +  "/" + std::to_string(registryIndex) + "_";

	// std::shared_ptr<SafeIndexManager> safeIndexManager = std::shared_ptr<SafeIndexManager>( new SafeIndexManager(_normal._indexManager->masterBtree()) );
	// SafeIndexManager* safeIndexManager = new SafeIndexManager( _normal._indexManager->masterBtree() );

	// セーフモードに移行する前のデータベース状態の保持
	std::shared_ptr<unsigned char> dbState;
	dbState = _normal._indexManager->oMemoryManager()->dbState();

	std::string safeIndexFilePath = safeDirPath + _dbName + "_index";
	std::shared_ptr<SafeIndexManager> safeIndexManager = std::shared_ptr<SafeIndexManager>(  new SafeIndexManager( safeIndexFilePath , _normal._indexManager->masterBtree() , dbState ) );
	safeIndexManager->clear();
	/* SafeIndexManager は内部で関連ファイルを削除&フォーマットするので明示的にclearする必要はない */

	// あとでファイル名はハードコードする
	std::string valueStoreFilePath = safeDirPath + _dbName;
	std::shared_ptr<ValueStoreManager> valueStoreManager = std::shared_ptr<ValueStoreManager>( new ValueStoreManager{ valueStoreFilePath });
	valueStoreManager->clear();


	_safe._activeSafeIndexManagerArray[registryIndex] = safeIndexManager;
	_safe._activeValueStoreManagerArray[registryIndex] = valueStoreManager;

	std::cout << "\x1b[33m" << "SafeIndexManager Added to Array with :: " << registryIndex << "\x1b[39m" << "\n";

	/*
	_indexManager = dynamic_cast<IndexManager*>( safeIndexManager.get() );
	_valueStoreManager = valueStoreManager.get();

	_safe._activeSafeIndexManagerArray[registryIndex] = safeIndexManager;
	_safe._activeValueStoreManagerArray[registryIndex] = valueStoreManager;

	_isSafeMode = true;

	std::cout << "セーフモードに切り替わりました" << "\n";
	std::cout << _isSafeMode << "\n";
	*/
}


bool MMyISAM::safeCommitExit( std::shared_ptr<QueryContext> qctx )
{
	short registryIndex = qctx->registryIndex();

	// debug
	this->showSafeModeState();



	if( registryIndex < 0 ) return false; // ノーマルモードが指定されている場合は特に何もしない
	std::shared_ptr<SafeIndexManager> safeIndexManager = _safe._activeSafeIndexManagerArray.at(registryIndex);
	if( safeIndexManager == nullptr ) return false; // 指定のsafeModeIndexManagerが存在しない

	auto valueStoreManager = _safe._activeValueStoreManagerArray.at(registryIndex);
	if( valueStoreManager == nullptr ) return false;

	/* Meta領域のコピ 雑すぎる　後で修正する */
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	std::shared_ptr<ONodeConversionTable> conversionTable = safeIndexManager->conversionTable();
	std::shared_ptr<optr> oNodeMeta = std::make_shared<optr>( addrZero , conversionTable->normalOMemoryManager()->dataCacheTable() );
	std::shared_ptr<optr> safeONodeMeta = std::make_shared<optr>( addrZero , conversionTable->safeOMemoryManager()->dataCacheTable() );
	omemcpy( oNodeMeta.get() , safeONodeMeta.get() , 100 );

	conversionTable->printEntryMap();

	/* Dataのコピー・移動 */
	_normal._valueStoreManager->mergeDataOptr( conversionTable , valueStoreManager.get() );

	/* ルートノード更新の反映 */
	std::shared_ptr<ONode> newRootONode = dynamic_cast<SafeIndexManager*>(safeIndexManager.get())->mergeSafeBtree(); // インデックスのコミット
	(_normal._indexManager)->masterBtree()->rootONode( newRootONode );
	// dynamic_cast<NormalIndexManager*>(_normal._indexManager.get())->masterBtree()->rootONode( newRootONode ); // マージ後ルートノードに変更があった場合はNormalにも変更を加える

	_normal._indexManager->oMemoryManager()->syncDBState();
	clearSafeMode();
	
  return true;
}


bool MMyISAM::safeAbortExit( std::shared_ptr<QueryContext> qctx )
{

  short registryIndex = qctx->registryIndex();
  if( registryIndex < 0 ) return false; // ノーマルモードの時は特に何もしない

  std::cout << "before::clearSafeMode()" << "\n";
  clearSafeMode();
  return true;
}


void MMyISAM::hello()
{
	std::cout << "Hello" << "\n";
}



void MMyISAM::showSafeModeState()
{
  std::cout << "[ Safe Mode Status ]" << "\n";
  for( int i=0; i< _safe._activeSafeIndexManagerArray.size() ; i++ )
  {
	if( _safe._activeSafeIndexManagerArray.at(i) == nullptr )
	  printf("RegistryIndex :: [%d](%s) \n", i , "\x1b[31m deActive \x1b[39m" );
	else
	  printf("RegistryIndex :: [%d](%s) \n", i, "\x1b[32m active \x1b[39m" );
  }
  return;
}




};
