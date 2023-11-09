#ifndef D7A0C05A_658A_402C_B637_3A25CE464170
#define D7A0C05A_658A_402C_B637_3A25CE464170



#include "../../components/index_manager/btree.h"



namespace miya_db
{

class SafeONode;



class SafeOBtree : public OBtree
{
private:
    std::shared_ptr<SafeONode> _rootONode;

public:
    SafeOBtree( std::shared_ptr<ONode> normalRootONode );
    const std::shared_ptr<SafeONode> rootONode();

    void hello();
};



};

#endif 
