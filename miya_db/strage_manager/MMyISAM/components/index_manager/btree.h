#ifndef A06C4298_986B_4258_90AC_11D64FF761CA
#define A06C4298_986B_4258_90AC_11D64FF761CA

#include <iostream>
#include <array>
#include <vector>
#include <memory>


#include "../page_table/overlay_ptr.h"




namespace miya_db
{






constexpr unsigned int DEFAULT_THRESHOLD = 5;
constexpr unsigned int KEY_SIZE = (128/8);

constexpr unsigned int DATA_OPTR_SIZE = 5;
constexpr unsigned int CHILD_OPTR_SIZE = 5;

constexpr unsigned int ELEMENT_COUNT_SIZE = 1;


class OBtreeNode;





// メモリ上へのインデックス情報配置は (個数)_key_key_key_key_key(個数)_dataPtr_dataPtr_dataPtr_dataPtr_dataPtr(個数)_child_child_child_child_child
// 個数はどうする？


// optrをラップして扱いやすくするための
struct ONodeItemSet
{
private:
	optr *_optr;


public:
	ONodeItemSet( optr *__optr  ){ _optr = __optr; };
	~ONodeItemSet(){ delete _optr; };

	unsigned short keyCount();
	unsigned short childCount();
	unsigned short dataOptrCount();

	std::unique_ptr<optr> key( unsigned short index );
	std::unique_ptr<ONodeItemSet> child( unsigned short index );
	std::unique_ptr<optr> dataOptr( unsigned short index );
};








class OBtreeNode
{
private:
	ONodeItemSet *_oNodeItemSet;

public:
	/* 肝となるメソッド2つ */
	void registIndex( unsigned char* key , optr *dataPtr , optr* leftChildNode = nullptr, optr* rightChildNode = nullptr );
	OBtreeNode* subtreeKeySearch( unsigned char *key );

};











class OBtree
{
private:
	OBtreeNode *_rootNode = nullptr;

public:
	OBtree();

	const OBtreeNode *rootNode();

	// void registerIndex( unsigned char* key , optr *dataOptr ); // 実際にはdataOptrのアドレスunsigned char[5]が格納される

};





/*


Index 
( key(128bit) - value(optr(40bit)) )


--------------------------------
===============================
|  KEY(SHA-1)    |       |      |      | 
===============================
--------------------------------


*/















}; // close miya_db namesapce





#endif // A06C4298_986B_4258_90AC_11D64FF761CA



