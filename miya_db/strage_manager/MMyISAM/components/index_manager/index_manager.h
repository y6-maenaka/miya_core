#ifndef CD358C3C_D694_4AD5_922A_7558220922B5
#define CD358C3C_D694_4AD5_922A_7558220922B5


#include <memory>
#include <iostream>

namespace miya_db{
class PageTableManager;
class OverlayMemoryManager;
class OBtreeNode;
class optr;



class IndexManager{

private:
	std::shared_ptr<OBtree> _masterBtree; // バックアップファイルから起動する場合は

public:

	IndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );

	// 基本的に操作系は２通り deleteは未実装
	void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr );
	std::shared_ptr<optr> find( std::shared_ptr<unsigned char> key );
}


}; // close miya_db namespace



#endif // CD358C3C_D694_4AD5_922A_7558220922B5

