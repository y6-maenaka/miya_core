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




class MMyISAM : public UnifiedStorageManager { // 継承したほうがいい?

private:	
	std::shared_ptr<OverlayMemoryManager> _dataOverlayMemoryManager; // データが保存されているファイルのマネージャー
	std::shared_ptr<IndexManager> _indexManager; // インデックスが保存されているマネージャーを渡す

public:
	MMyISAM( std::string fileName );

	void regist( std::shared_ptr<unsigned char> data , size_t dataLength , std::shared_ptr<unsigned char> key ); // メソッド名が微妙
	std::shared_ptr<optr> get( std::shared_ptr<unsigned char> key );

};

}; // close miya_db namespace



#endif // CD595D1E_AA1A_439E_9D55_12032E706D4D
