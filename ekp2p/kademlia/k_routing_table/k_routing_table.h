#ifndef DE5E77B9_E6DA_4D13_8A08_65141CCB994F
#define DE5E77B9_E6DA_4D13_8A08_65141CCB994F



#include <array>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>



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
class SocketManager;







class KRoutingTable
{
private:
	std::shared_ptr<KHostNode> _hostNode;
	std::array< std::shared_ptr<KBucket> , K_BUCKET_SIZE > _bucketArray;

public:
	KRoutingTable( std::shared_ptr<SocketManager> target );

	int autoAdd( std::shared_ptr<KNodeAddr> target );

	std::shared_ptr<KBucket> kBucket( unsigned short branch );
	short int calcBranch( std::shared_ptr<KNodeAddr> targetNodeAddr );

	std::vector< const std::shared_ptr<KClientNode> > randomPick( size_t size );

	void notifyNodeSwap( std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > target );
	std::shared_ptr<KHostNode> hostNode(); 
	
};



/* RoutingTable Update種類

	(bucket内に要素が存在する) - 最後尾に移動

	(bucket内に要素が存在しない)  (満杯) : 最後尾に移動
							  (非満杯) : PINGの応答次第
 
	--------------------------------------------------------------------------

	(非満杯場合)  - bucket内に対象のNodeが存在しない場合 :: バケット最後尾に移動
				- bucket内に対象のNodeが存在しない場合 :: バケット最後尾に移動

	(満杯場合)    - bucket内に対象のNodeが存在する場合  :: バケット最後尾に移動
				 - bucket内に対象のNodeが存在しない場合  :: 先頭要素にPINGを打って応答次第で最後尾に追加

*/



};




#endif // DE5E77B9_E6DA_4D13_8A08_65141CCB994F

