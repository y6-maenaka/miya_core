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




class MMyISAM //: public UnifiedStorageManager { // 継承したほうがいい? 継承したメソッドを呼び出すとエラーが発生する
{
//private:
public:
	// std::shared_ptr<OverlayMemoryManager> _dataOverlayMemoryManager; // データが保存されているファイルのマネージャー
	std::shared_ptr<ValueStoreManager>	_valueStoreManager;
	std::shared_ptr<IndexManager> _indexManager; // インデックスが保存されているマネージャーを渡す

//public:
	MMyISAM( std::string fileName );

	bool add( std::shared_ptr<QueryContext> qctx );
	bool get( std::shared_ptr<QueryContext> qctx );

	void hello();

};

}; // close miya_db namespace



#endif // CD595D1E_AA1A_439E_9D55_12032E706D4D
