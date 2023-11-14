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


MMyISAM::MMyISAM( std::string filePath )
{
   // データストアの初期化
	_normal._valueStoreManager = std::make_shared<ValueStoreManager>( filePath );

    // インデックスの初期化
    filePath += "_index";
	_normal._indexManager = std::make_shared<NormalIndexManager>( filePath );

	// 初回起動時は通常モードで起動する
	_valueStoreManager = _normal._valueStoreManager.get();
	_indexManager = dynamic_cast<IndexManager*>(_normal._indexManager.get());
}




bool MMyISAM::add( std::shared_ptr<QueryContext> qctx )
{
    // データの保存
	std::shared_ptr<optr> storeTarget = _valueStoreManager->add( qctx ); // 先にデータ()を配置する

	// インデックスの作成
    _indexManager->add( qctx->key() , storeTarget );

	std::cout << "\x1b[36m" << "(MiyaDB) add with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";


	/*
	std::cout << "-----------------------------------------------------------------------" << "\n";
	std::cout << "( add )" << "\n";
	std::cout << "[ NormalIndexManager ]" << "\n";
	_indexManager->printIndexTree();
	std::cout << "-----------------------------------------------------------------------" << "\n";
	std::cout << "[ IndexManager ( Normal or Safe ) ] :: ";
	if( _isSafeMode ) std::cout << "safe" << "\n";
	else std::cout << "normal"  << "\n";
	std::cout << "-----------------------------------------------------------------------" << "\n";
	*/

	return true;
}



bool MMyISAM::get( std::shared_ptr<QueryContext> qctx )
{
	std::pair< std::shared_ptr<optr>, int > dataOptr = _indexManager->find( qctx->key() ); // ここで取得されるoptrにはキャッシュテーブルがセットされていない

	std::cout << "\x1b[32m" << "(MiyaDB) get with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i]);
	} std::cout << "\x1b[39m" << "\n";


	if( dataOptr.first == nullptr ) return false;
	size_t dataLength; std::shared_ptr<unsigned char> data;

	/* あまり良くないデータストアの切り替え方法 */
	if( dataOptr.second == 0 ) // SafeModeで0以外が返却されることは実装上ありあえないので大丈夫だと思う
		dataLength = _normal._valueStoreManager->get( dataOptr.first , &data );
	if( dataOptr.second == 1 ) // Safeに存在する場合
		dataLength = _valueStoreManager->get( dataOptr.first , &data );

	qctx->value( data , dataLength );

	return true;
}





bool MMyISAM::remove( std::shared_ptr<QueryContext> qctx )
{
	std::cout << "\x1b[35m" << "(MiyaDB) remove with key :: ";
	for( int i=0; i<20; i++){
		printf("%02X", qctx->key().get()[i] );
	}  std::cout << "\x1b[39m"  << "\n";

	_indexManager->remove( qctx->key() );
 // 本来はvalueStoreからも削除する

	/*
	std::cout << "-----------------------------------------------------------------------" << "\n";
	std::cout << "( REMOVE )" << "\n";
	std::cout << "[ NormalIndexManager ]" << "\n";
	_indexManager->printIndexTree();
	std::cout << "-----------------------------------------------------------------------" << "\n";
	std::cout << "[ IndexManager ( Normal or Safe ) ] :: ";
	if( _isSafeMode ) std::cout << "safe" << "\n";
	else std::cout << "normal"  << "\n";
	std::cout << "-----------------------------------------------------------------------" << "\n";
	*/


	return true;
}




bool MMyISAM::exists( std::shared_ptr<QueryContext> qctx )
{
	std::pair< std::shared_ptr<optr> , int > dataOptr = _indexManager->find( qctx->key() );

	// 簡易的にキーが存在するか否かでデータの存在有無を判別することとする
	if( dataOptr.first == nullptr ) return false; // キーが存在しない : false
	return true; // キーが存在しない : true
}



bool MMyISAM::switchToSafeMode()
{
	if( _isSafeMode ) return false;

	//std::shared_ptr<SafeIndexManager> safeIndexManager = std::shared_ptr<SafeIndexManager>( new SafeIndexManager(_normal._indexManager->masterBtree()) );
	SafeIndexManager* safeIndexManager = new SafeIndexManager( _normal._indexManager->masterBtree() );
	/* SafeIndexManager は内部で関連ファイルを削除&フォーマットするので明示的にclearする必要はない */

	// あとでファイル名はハードコードする
	ValueStoreManager* valueStoreManager = new ValueStoreManager{std::string("../miya_db/table_files/.system/safe/safe")};
	valueStoreManager->clear();

	_indexManager = dynamic_cast<IndexManager*>( safeIndexManager );
	_valueStoreManager = valueStoreManager;

	_isSafeMode = true;

	std::cout << "セーフモードに切り替わりました" << "\n";
	std::cout << _isSafeMode << "\n";
}


bool MMyISAM::safeCommitExit()
{
	if( !_isSafeMode ) return false;

	/* Meta領域のコピ 雑すぎる　後で修正する */
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	std::shared_ptr<optr> onodeMeta = std::make_shared<optr>( addrZero , SafeONode::_conversionTable.normalOMemoryManager()->dataCacheTable() );
	std::shared_ptr<optr> safeONodeMeta = std::make_shared<optr>( addrZero , SafeONode::_conversionTable.safeOMemoryManager()->dataCacheTable() );
	omemcpy( onodeMeta.get() , safeONodeMeta.get() , 100 );

	SafeONode::_conversionTable.printEntryMap();

	/* Dataのコピー・移動 */
	_normal._valueStoreManager->mergeDataOptr( _valueStoreManager );

	/* ルートノード更新の反映 */
	std::shared_ptr<ONode> newRootONode = dynamic_cast<SafeIndexManager*>(_indexManager)->mergeSafeBtree(); // インデックスのコミット
	dynamic_cast<NormalIndexManager*>(_normal._indexManager.get())->masterBtree()->rootONode( newRootONode );


	delete _indexManager;
	delete _valueStoreManager;

	_indexManager = dynamic_cast<IndexManager*>( _normal._indexManager.get() );
	_valueStoreManager = dynamic_cast<ValueStoreManager*>( _normal._valueStoreManager.get() );


	_isSafeMode = false;
}


bool MMyISAM::safeAbortExit()
{
	if( !_isSafeMode ) return false;


	delete _indexManager;
	delete _valueStoreManager;

	_indexManager = dynamic_cast<IndexManager*>( _normal._indexManager.get() );
	_valueStoreManager = dynamic_cast<ValueStoreManager*>( _normal._valueStoreManager.get() );

	_isSafeMode = false;
}



void MMyISAM::hello()
{
	std::cout << "Hello" << "\n";
}



};
