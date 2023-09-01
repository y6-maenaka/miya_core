#ifndef DE5E77B9_E6DA_4D13_8A08_65141CCB994F
#define DE5E77B9_E6DA_4D13_8A08_65141CCB994F



#include <array>
#include <memory>



namespace ekp2p
{


constexpr unsigned short int K_BUCKET_SIZE = 160;
constexpr int NODE_ID_LENGTH = 20;
// constexpr unsigned short DEFAULT_FIND_NODE_SIZE = 5;

// typedef std::bitset<160> BitSet_160; // prepare 20 bytes std::bitset


struct KBucket;
struct KNodeAddr;
class KClientNode;
class KHostNode;







class KRoutingTable
{
private:
	std::shared_ptr<KHostNode> _hostNode;
	std::array< std::shared_ptr<KBucket> , K_BUCKET_SIZE > _routingTable;

public:
	KRoutingTable( std::shared_ptr<KHostNode> hostNode );

	int autoAdd( KNodeAddr *target );

	std::shared_ptr<KBucket> kBucket( unsigned short branch );
	short int calcBranch( KNodeAddr *targetNodeAddr );
};



/*
 ルーティングテーブルに必要な機能
 
 - アップデート機能( KRoutingTable Updator )
 - 
*/



};


#endif // DE5E77B9_E6DA_4D13_8A08_65141CCB994F

