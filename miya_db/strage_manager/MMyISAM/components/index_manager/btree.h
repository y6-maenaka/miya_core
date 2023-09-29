#ifndef A06C4298_986B_4258_90AC_11D64FF761CA
#define A06C4298_986B_4258_90AC_11D64FF761CA

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <array>
#include <algorithm>


#include "../page_table/overlay_ptr.h"
#include "../page_table/overlay_memory_manager.h"




namespace miya_db
{



/*
 ------------------------------------------------------------------------------------------
 | 親ONodeへのポインタ | keyの個数|key|key| child個数 | child| child| dataPtr個数| dataPtr
 ------------------------------------------------------------------------------------------
 */




constexpr unsigned int DEFAULT_THRESHOLD = 3;// 本来は5でいく
constexpr unsigned int DEFAULT_CHILD_COUNT = DEFAULT_THRESHOLD; // 子の個数
constexpr unsigned int DEFAULT_KEY_COUNT = DEFAULT_THRESHOLD - 1;
constexpr unsigned int DEFAULT_DATA_OPTR_COUNT = DEFAULT_THRESHOLD - 1;



constexpr unsigned int KEY_SIZE = (160/8); // SHA1 20 [ bytes ]

constexpr unsigned int DATA_OPTR_SIZE = 5; // [ bytes ]
constexpr unsigned int NODE_OPTR_SIZE = 5; // [ bytes ]

constexpr unsigned int ELEMENT_COUNT_SIZE = 1;
 


/* 要素の個数からのポインタ */
constexpr unsigned int KEY_ELEMENT_OFFSET = NODE_OPTR_SIZE; // 親ノードへのポインタ
constexpr unsigned int CHILD_ELEMENT_OFFSET = KEY_ELEMENT_OFFSET + (ELEMENT_COUNT_SIZE + (KEY_SIZE*DEFAULT_KEY_COUNT));
constexpr unsigned int DAYA_OPTR_ELEMENT_OFFSET = CHILD_ELEMENT_OFFSET + (ELEMENT_COUNT_SIZE) + (DATA_OPTR_SIZE*DEFAULT_CHILD_COUNT);


constexpr unsigned int O_NODE_ITEMSET_SIZE = NODE_OPTR_SIZE  /* 親ノードへのポインタ */
																						+ ELEMENT_COUNT_SIZE + (KEY_SIZE * DEFAULT_KEY_COUNT) /* キーのサイズ */
																						+ ELEMENT_COUNT_SIZE + (NODE_OPTR_SIZE * DEFAULT_CHILD_COUNT)
																						+ ELEMENT_COUNT_SIZE + (DATA_OPTR_SIZE * DEFAULT_DATA_OPTR_COUNT);

//class OBtreeNode;
class ONode;
struct ONodeItemSet;











struct ViewItemSet // ノードの追加の時際,分割をサポートする仮想的なアイテムセット 
									 // ※少々オーバヘッドが大きいため,分割の際だけに使用する
{
	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT+1> _key; // 分割の際に一体対象のキーも追加するためサイズはプラス１
	std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT+1> _child;

	//void importItemSet( ONodeItemSet *itemSet );
	void importItemSet( std::shared_ptr<ONodeItemSet> itemSet );
	void moveInsert( unsigned short index ,std::shared_ptr<ONode> target );
};







// メモリ上へのインデックス情報配置は [親ノードへのポインタ](個数)_key_key_key_key_key(個数)_dataPtr_dataPtr_dataPtr_dataPtr_dataPtr(個数)_child_child_child_child_child
// 個数はどうする？


// optrをラップして扱いやすくするための
struct ONodeItemSet
{
private:
	//optr *_optr;
	std::shared_ptr<optr> _optr = nullptr; // このItemSetの起点となる

public:
	ONodeItemSet( std::shared_ptr<optr> __optr );


	std::shared_ptr<optr> Optr();

	std::shared_ptr<optr> parent();

	unsigned short keyCount();
	void keyCount( unsigned short num );

	unsigned short childCount();
	void childCount( unsigned short num );

	unsigned short dataOptrCount();

	std::shared_ptr<optr> key( unsigned short index );
	void key( unsigned short index , std::shared_ptr<unsigned char> targetKey ); // setter
	void sortKey();

	//std::unique_ptr<ONode> child( unsigned short index );
	std::shared_ptr<ONode> child( unsigned short index );
	void child( unsigned short index , std::shared_ptr<ONode> targetONode );
	void moveInsert( unsigned short index , std::shared_ptr<ONode> targetONode );

	std::shared_ptr<optr> dataOptr( unsigned short index );


	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *exportKeyArray();
	std::array< std::shared_ptr<ONode> , DEFAULT_CHILD_COUNT> *exportChildArray();

	void clear();
};








class ONode : public std::enable_shared_from_this<ONode>
{
private:
	std::shared_ptr<ONodeItemSet> _itemSet;
	std::shared_ptr<OverlayMemoryManager> _oMemoryManager;
	bool _isLeaf = true;

public:
	ONode( std::shared_ptr<optr> baseOptr );
	ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr );

	std::shared_ptr<ONode> parent();
	//void parent( ONode* target );
	void parent( std::shared_ptr<ONode> target );
	std::shared_ptr<ONode> child( unsigned short index );

	std::shared_ptr<ONodeItemSet> itemSet();

	// ルートノードが更新されるとONodeがリターンされる
	std::shared_ptr<ONode> recursiveAdd( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<ONode> targetONode = nullptr );
	std::shared_ptr<optr> subtreeFind( std::shared_ptr<unsigned char> targetKey );
	//void add( 
	unsigned char* splitONode( unsigned char* targetKey );
	

	void isLeaf( bool flag ){ _isLeaf = flag; };
	bool isLeaf(){ return _isLeaf; }; // getter

	void splitONode(); // キー数が閾値を超えるとスプリットする

	void overlayMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	std::shared_ptr<OverlayMemoryManager> overlayMemoryManager();

	/* 肝となるメソッド2つ */
	//void registIndex( unsigned char* key , optr *dataPtr , optr* leftChildNode = nullptr, optr* rightChildNode = nullptr );
	void regist( unsigned char* targetKey , optr *targetDataOptr );


	// 被ラップ関係からのキャストだからあまりよくない
	std::unique_ptr<ONode> subtreeKeySearch( ONode* targetONode,unsigned char *targetKey );  // 挿入一のノードを検索してくる


};











class OBtree
{
private:
	//ONode *_rootONode = nullptr;
	std::shared_ptr<ONode> _rootONode;

public:
	OBtree( std::shared_ptr<OverlayMemoryManager> oMemoryManager ,  std::shared_ptr<ONode> rootNode = nullptr );
	void add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<ONode> targetONode  = nullptr );
	std::shared_ptr<optr> find( std::shared_ptr<unsigned char> targetKey );

	void rootONode( std::shared_ptr<ONode> target );
	const std::shared_ptr<ONode> rootONode();
	static int printSubTree( std::shared_ptr<ONode> subtreeRoot );
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



