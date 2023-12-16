#ifndef D7A0C05A_658A_402C_B637_3A25CE464170
#define D7A0C05A_658A_402C_B637_3A25CE464170



#include "../components/index_manager/btree.h"



namespace miya_db
{

class SafeONode;
struct ONodeConversionTable;


using DataOptrEx = std::pair< std::shared_ptr<optr> , int >; // データ本体先頭Optrとデータファイルインデックス

class SafeOBtree : public OBtree
{
private:
    std::shared_ptr<SafeONode> _rootONode;
    std::shared_ptr<ONodeConversionTable> _conversionTable;

public:
    SafeOBtree( std::shared_ptr<ONodeConversionTable> conversionTable ,std::shared_ptr<ONode> normalRootONode );
    const std::shared_ptr<SafeONode> rootONode();
    std::shared_ptr<ONodeConversionTable> conversionTable();

    void add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr ) override;
		void remove( std::shared_ptr<unsigned char> targetKey ) override;
		std::shared_ptr<DataOptrEx> find( std::shared_ptr<unsigned char> targetKey );
  
    // セーフモードでの変更を通常Obtreeに反映する ≒ commit
		std::shared_ptr<ONode> mergeSafeBtree( std::shared_ptr<SafeONode> subtreeRootONode , short int childIndex = -1  ,std::shared_ptr<ONode> parentONode = nullptr ); 

    static int printONode( std::shared_ptr<SafeONode> targetONode );
		static int printSubTree( std::shared_ptr<SafeONode> subtreeRoot );

    void hello();
};



};

#endif
