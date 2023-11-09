#ifndef CD595D1E_AA1A_439E_9D55_12032E706D4D
#define CD595D1E_AA1A_439E_9D55_12032E706D4D

#include <memory>
#include <string>

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




class MMyISAM //: public UnifiedStorageManager { // 継承したほうがいい? 継承したメソッドを呼び出すとエラーが発生する
{
private:
//public:
	// std::shared_ptr<OverlayMemoryManager> _dataOverlayMemoryManager; // データが保存されているファイルのマネージャー
	ValueStoreManager*_valueStoreManager;
	IndexManager* _indexManager; // インデックスが保存されているマネージャーを渡す


	struct 
	{
		struct {
			std::shared_ptr<ValueStoreManager> _valueStoreManager = nullptr;
			std::shared_ptr<NormalIndexManager> _indexManager = nullptr;
		} _normal;

		struct { // safe関係モジュールは毎回初期化されるので,持つ必要ないかも
			// std::shared_ptr<ValueStoreManager> _valueStoreManager = nullptr;
			//std::shared_ptr<SafeIndexManager> _indexManager = nullptr;
		} _safe;
	};

	bool _isSafeMode = false;

public:
	MMyISAM( std::string filePath );

	bool add( std::shared_ptr<QueryContext> qctx );
	bool get( std::shared_ptr<QueryContext> qctx );
	bool remove( std::shared_ptr<QueryContext> qctx );
	bool exists( std::shared_ptr<QueryContext> qctx );


	bool switchToSafeMode(); // セーフモードに移行する ※トランザクションのイメージ
	bool safeCommitExit(); // セーフモードを本ファイルに同期して終了する
	bool safeAbortExit();  // セーフモードを破棄して終了する

	void hello();
};

}; // close miya_db namespace



#endif // CD595D1E_AA1A_439E_9D55_12032E706D4D
