#include "safe_btree.h"



namespace miya_db
{









SafeOBtree::SafeOBtree( std::shared_ptr<ONode> normalRootONode ) : OBtree( normalRootONode )
{
    _rootONode = static_pointer_cast<SafeONode>( OBtree::_rootONode );

}


const std::shared_ptr<SafeONode> SafeOBtree::rootONode()
{
    return _rootONode;
}






}