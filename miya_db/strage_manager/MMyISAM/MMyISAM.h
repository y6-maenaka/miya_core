#ifndef CD595D1E_AA1A_439E_9D55_12032E706D4D
#define CD595D1E_AA1A_439E_9D55_12032E706D4D

#include <memory>
#include <string>
#include <vector>

//#include "../unified_strage_engine/unified_strage_engine.h"
#include "../unified_storage_manager/unified_storage_manager.h"


namespace miya_db{


class OverlayMemoryManager;
class IndexManager;
class optr;
class ValueStoreManager;
struct QueryContext;
class NormalIndexManager;
class SafeIndexManager;
class SafeValueStoreManager;

constexpr unsigned short ALLOWED_SAFE_REGISTRY_COUNT = 5;


class MMyISAM //: public UnifiedStorageManager { // 継承したほうがいい? 継承したメソッドを呼び出すとエラーが発生する
{
private:
//public:
	// std::shared_ptr<OverlayMemoryManager> _dataOverlayMemoryManager; // データが保存されているファイルのマネージャー
	// ValueStoreManager*_valueStoreManager;
	// IndexManager* _indexManager; // インデックスが保存されているマネージャーを渡す
	std::string _dbName;

	struct Normal
	{
		std::shared_ptr<NormalIndexManager> _indexManager = nullptr;
		std::shared_ptr<ValueStoreManager> _valueStoreManager = nullptr;
  
	} _normal;

	struct Safe // safe関係モジュールは毎回初期化されるので,持つ必要ないかも
	{ 
		std::array< std::shared_ptr<SafeIndexManager>,  ALLOWED_SAFE_REGISTRY_COUNT > _activeSafeIndexManagerArray = {{nullptr}};
		std::array< std::shared_ptr<ValueStoreManager>, ALLOWED_SAFE_REGISTRY_COUNT > _activeValueStoreManagerArray = {{nullptr}};

		void clear(); // 全てのSafeIndexManagerとValueStoreManagerをクエリする
	} _safe;

	void clearSafeMode();

public:
	MMyISAM( std::string dbName );

	bool add( std::shared_ptr<QueryContext> qctx );
	bool get( std::shared_ptr<QueryContext> qctx );
	bool remove( std::shared_ptr<QueryContext> qctx );
	bool exists( std::shared_ptr<QueryContext> qctx );

	bool migrateToSafeMode( std::shared_ptr<QueryContext> qctx ); // セーフモードに移行する ※トランザクションのイメージ
	bool safeCommitExit( std::shared_ptr<QueryContext> qctx ); // セーフモードを本ファイルに同期して終了する
	bool safeAbortExit( std::shared_ptr<QueryContext> qctx );  // セーフモードを破棄して終了する

	void showSafeModeState();
	void hello();
};

}; // close miya_db namespace



#endif // CD595D1E_AA1A_439E_9D55_12032E706D4D
