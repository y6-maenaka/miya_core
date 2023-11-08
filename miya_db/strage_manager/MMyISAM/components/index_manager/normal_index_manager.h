#ifndef CD358C3C_D694_4AD5_922A_7558220922B5
#define CD358C3C_D694_4AD5_922A_7558220922B5

#include "./index_manager.h"

#include <memory>
#include <iostream>


namespace miya_db{
class PageTableManager;
class OverlayMemoryManager;
class OBtreeNode;
class OBtree;
class optr;



class NormalIndexManager : public IndexManager
{

//private:
public:
	std::shared_ptr<OBtree> _masterBtree; // バックアップファイルから起動する場合は

//public:

	NormalIndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );

	// 基本的に操作系は２通り deleteは未実装
	void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr );
	void remove( std::shared_ptr<unsigned char> key );
	std::shared_ptr<optr> find( std::shared_ptr<unsigned char> key );
};


}; // close miya_db namespace



#endif // CD358C3C_D694_4AD5_922A_7558220922B5
