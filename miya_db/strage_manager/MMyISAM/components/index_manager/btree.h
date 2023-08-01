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
constexpr unsigned int INDEX_KEY_SIZE = (128/8);




class OBtreeNode;





// メモリ上へのインデックス情報配置は _key_key_key_key_key _dataPtr_dataPtr_dataPtr_dataPtr_dataPtr _child_child_child_child_child

struct ONodeItemSet
{
private:
	optr *_optr;

	/* いっそのこと配列でoptrを持つ必要はない？
	struct ItemSet{
		struct 
			{
				std::array< unsigned char*, DEFAULT_THRESHOLD > _key;	 // size -> (16 * 5)
				std::array< unsigned char*, DEFAULT_THRESHOLD > _dataOptr; //  size -> (5*5)
			} _keyPair;

			std::array< unsigned char* , DEFAULT_THRESHOLD + 1 > _child; // 子ノードへのポインタ size -> (5*6)
	} _itemSet;
	*/

public:

	unsigned short keyCount();
	unsigned short childCount();
	unsigned short dataPtrCount();

	std::unique_ptr<optr> key( unsigned short index );
	OBtreeNode* child( unsigned short index );
	std::unique_ptr<optr> dataPtr( unsigned short index );

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



