#include "safe_btree.h"

#include "./safe_o_node.h"


namespace miya_db
{






SafeOBtree::SafeOBtree( std::shared_ptr<ONode> normalRootONode ) : OBtree( nullptr ,normalRootONode )
{
    SafeONode *castedNormalONode = static_cast<SafeONode*>( normalRootONode.get() );
    _rootONode = std::make_shared<SafeONode>( *castedNormalONode );

    SafeONode::_conversionTable.init(); // entryMapを削除する

    (static_pointer_cast<SafeONode>( _rootONode ))->hello();
    _rootONode->hello();
    SafeOBtree::_rootONode->hello();
    std::cout << "\x1b[35m" << "SafeOBTreeが初期化されました" << "\x1b[39m" << "\n";
}


const std::shared_ptr<SafeONode> SafeOBtree::rootONode()
{
    return _rootONode;

}


void SafeOBtree::hello()
{
    _rootONode->hello();
}




}