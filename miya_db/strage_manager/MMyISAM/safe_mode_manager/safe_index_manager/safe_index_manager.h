#ifndef BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A
#define BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A



#include <iostream>
#include <memory>
#include <string>


#include "../../components/index_manager/index_manager.h"


namespace miya_db
{



class SafeOBtree;
class ONode;
class OBtree;





class SafeIndexManager : public IndexManager
{
private:
	std::shared_ptr<SafeOBtree> _masterBtree;

public:
	 // コンストラクタ実行時に必ずNormalIndexManagerのマッピング位置の倍数にならない様にダミー領域をはじめに確保する
	SafeIndexManager( const std::shared_ptr<OBtree> normalBtree );
	virtual ~SafeIndexManager(){};

	void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr ) override;
	void remove( std::shared_ptr<unsigned char> key ) override;
	std::shared_ptr<optr> find( std::shared_ptr<unsigned char> key ) override;

	void printIndexTree();
};






};


#endif //  BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A



