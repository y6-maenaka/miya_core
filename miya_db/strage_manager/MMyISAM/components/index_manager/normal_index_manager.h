#ifndef CD358C3C_D694_4AD5_922A_7558220922B5
#define CD358C3C_D694_4AD5_922A_7558220922B5

#include "./index_manager.h"

#include <memory>
#include <iostream>
#include <string>


namespace miya_db{
class PageTableManager;
class OverlayMemoryManager;
class OBtreeNode;
class OBtree;
class optr;



class NormalIndexManager : public IndexManager
{

private:
	std::shared_ptr<OBtree> _masterBtree; // バックアップファイルから起動する場合は
	// std::shared_ptr<OverlayMemoryManager> _oMemoryManager;

public:
	const std::shared_ptr<OBtree> masterBtree();

	NormalIndexManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager ); // 基本的にこのコンストラクタは使わない
	NormalIndexManager( std::string indexFilePath );
	virtual ~NormalIndexManager(){};

	// 基本的に操作系は２通り deleteは未実装
	void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr ) override;
	void remove( std::shared_ptr<unsigned char> key ) override;
	// (first): データの保存先Optr, (second): データファイルのインデックス(0-Normal ,1-Safe)
	std::pair< std::shared_ptr<optr> , int > find( std::shared_ptr<unsigned char> key ) override;

	void printIndexTree();
};


}; // close miya_db namespace



#endif // CD358C3C_D694_4AD5_922A_7558220922B5
