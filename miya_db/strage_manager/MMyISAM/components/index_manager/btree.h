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



#define FORMAT_CODE "HELLOMIYACOININDEX!!"



namespace miya_db
{



/*
 ------------------------------------------------------------------------------------------
 | 親ONodeへのポインタ | keyの個数|key|key| child個数 | child| child| dataPtr個数| dataPtr
 ------------------------------------------------------------------------------------------
 */



constexpr unsigned int DEFAULT_THRESHOLD = 4;// 本番は5でいく
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
constexpr unsigned int DATA_OPTR_ELEMENT_OFFSET = CHILD_ELEMENT_OFFSET + (ELEMENT_COUNT_SIZE) + (DATA_OPTR_SIZE*DEFAULT_CHILD_COUNT);


constexpr unsigned int O_NODE_ITEMSET_SIZE = NODE_OPTR_SIZE  /* 親ノードへのポインタ */
																						+ ELEMENT_COUNT_SIZE + (KEY_SIZE * DEFAULT_KEY_COUNT) /* キーのサイズ */
																						+ ELEMENT_COUNT_SIZE + (NODE_OPTR_SIZE * DEFAULT_CHILD_COUNT)
																						+ ELEMENT_COUNT_SIZE + (DATA_OPTR_SIZE * DEFAULT_DATA_OPTR_COUNT);


constexpr unsigned int META_ROOT_NODE_OFFSET = 20; // 定数を使う





//class OBtreeNode;
class ONode;
struct ONodeItemSet;









class OBtree
{
private:

protected:
	std::shared_ptr<ONode> _rootONode;
	std::shared_ptr<OverlayMemoryManager> _oMemoryManager;

public:
	OBtree( std::shared_ptr<OverlayMemoryManager> oMemoryManager ,  std::shared_ptr<ONode> rootNode = nullptr );
	// OBtree( std::shared_ptr<ONode> rootNode = nullptr );

	virtual void add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr );
	virtual void remove( std::shared_ptr<unsigned char> targetKey );
	std::shared_ptr<optr> find( std::shared_ptr<unsigned char> targetKey );

	void rootONode( std::shared_ptr<ONode> target );
	const std::shared_ptr<ONode> rootONode();

	const std::shared_ptr<OverlayMemoryManager> overlayMemoryManager();

	static int printONode( std::shared_ptr<ONode> targetONode );
	static int printSubTree( std::shared_ptr<ONode> subtreeRoot );
};














}; // close miya_db namesapce





#endif // A06C4298_986B_4258_90AC_11D64FF761CA
