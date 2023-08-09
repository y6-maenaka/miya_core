#ifndef A06C4298_986B_4258_90AC_11D64FF761CA
#define A06C4298_986B_4258_90AC_11D64FF761CA

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <array>
#include <algorithm>

#include "../page_table/overlay_ptr.h"




namespace miya_db
{






constexpr unsigned int DEFAULT_THRESHOLD = 5;
constexpr unsigned int DEFAULT_CHILD_COUNT = DEFAULT_THRESHOLD;
constexpr unsigned int DEFAULT_KEY_COUNT = DEFAULT_THRESHOLD - 1;
constexpr unsigned int DEFAULT_DATA_OPTR_COUNT = DEFAULT_THRESHOLD - 1;


constexpr unsigned int KEY_SIZE = (128/8);

constexpr unsigned int DATA_OPTR_SIZE = 5;
constexpr unsigned int NODE_OPTR_SIZE = 5;

constexpr unsigned int ELEMENT_COUNT_SIZE = 1;


//class OBtreeNode;
class ONode;
struct ONodeItemSet;











struct ViewItemSet // ノードの追加の時際,分割をサポートする仮想的なアイテムセット 
									 // ※少々オーバヘッドが大きいため,分割の際だけに使用する
{
	std::array<unsigned char* , DEFAULT_KEY_COUNT+1> *_key; // 分割の際に一体対象のキーも追加するためサイズはプラス１
	std::array<ONode* , DEFAULT_CHILD_COUNT+1> *_child;

	void importItemSet( ONodeItemSet *itemSet );
};







// メモリ上へのインデックス情報配置は [親ノードへのポインタ](個数)_key_key_key_key_key(個数)_dataPtr_dataPtr_dataPtr_dataPtr_dataPtr(個数)_child_child_child_child_child
// 個数はどうする？


// optrをラップして扱いやすくするための
struct ONodeItemSet
{
private:
	optr *_optr;

public:
	ONodeItemSet(){ _optr = nullptr; }; // メモリアロケータでアドレスをもらう
	ONodeItemSet( optr *__optr  ){ _optr = __optr; };
	~ONodeItemSet(){ delete _optr; };

	optr* Optr(){ return _optr; };

	unsigned short keyCount();
	void keyCount( unsigned short num );

	unsigned short childCount();
	unsigned short dataOptrCount();

	std::unique_ptr<optr> key( unsigned short index );
	void key( unsigned short index , unsigned char* targetKey ); // setter
	std::unique_ptr<ONode> child( unsigned short index );
	std::unique_ptr<optr> dataOptr( unsigned short index );


	std::array<optr*, DEFAULT_KEY_COUNT> *exportKeyOptrArray();
	std::array<unsigned char*, DEFAULT_KEY_COUNT> *exportKeyArray();
	std::array<ONode*, DEFAULT_CHILD_COUNT> *exportChildArray();
};








class ONode
{
private:
	ONodeItemSet *_itemSet;
	bool _isLeaf = true;

public:
	ONode(){ _itemSet = new ONodeItemSet; };
	ONode( optr* optr );
	ONode( ONodeItemSet *itemSet );
	~ONode(){ delete _itemSet; }; // 未定義エラー起こす可能性あり

	std::unique_ptr<ONode> parent(); // getter
	ONodeItemSet* itemSet(){ return _itemSet; }; // getter

	void add( unsigned char *targetKey , ONode* targetONode = nullptr );
	unsigned char* splitONode( unsigned char* targetKey );

	


	ONodeItemSet* oNodeItemSet(){ return _itemSet; }; // getter
	bool isLeaf(){ return _isLeaf; }; // getter

	void splitONode(); // キー数が閾値を超えるとスプリットする



	/* 肝となるメソッド2つ */
	//void registIndex( unsigned char* key , optr *dataPtr , optr* leftChildNode = nullptr, optr* rightChildNode = nullptr );
	void regist( unsigned char* targetKey , optr *targetDataOptr );

	// 被ラップ関係からのキャストだからあまりよくない
	std::unique_ptr<ONode> subtreeKeySearch( ONode* targetONode,unsigned char *targetKey );  // 挿入一のノードを検索してくる
};











class OBtree
{
private:
	ONode *_rootONode = nullptr;

public:
	OBtree( ONode *oNode );

	const ONode *rootNode();

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



