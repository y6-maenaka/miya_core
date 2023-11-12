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

    void add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr ) override;
	void remove( std::shared_ptr<unsigned char> targetKey ) override;

    static int printONode( std::shared_ptr<SafeONode> targetONode );
	static int printSubTree( std::shared_ptr<SafeONode> subtreeRoot );
    void hello();
};



};

#endif 
