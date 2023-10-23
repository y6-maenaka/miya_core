#ifndef E1BCBE66_37FA_47FD_AF31_933653808E4D
#define E1BCBE66_37FA_47FD_AF31_933653808E4D


#include <array>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <functional>






namespace ekp2p
{


constexpr unsigned short K_SIZE = 8;
struct KNodeAddr;
class KClientNode;



using KClientNodeVector = std::vector< std::shared_ptr<KClientNode> >;


struct KBucket : public std::enable_shared_from_this<KBucket>
{
private:
	// std::mutex _mtx; 保留,std::unique_ptrで対応できないか検討する
	// std::vector< std::shared_ptr<KClientNode> > _nodeVector;
	KClientNodeVector _nodeVector;
	std::vector< std::shared_ptr<KClientNode> >::iterator _referenceHead;

	// 排他制御
	std::mutex _mtx;

	std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > _notifyNodeSwap;

protected:
	std::shared_ptr<KClientNode> find( std::shared_ptr<KClientNode> target );
	// std::shared_ptr<KClientNode> find( std::shared_ptr<KClientNode> target );
	// int bucketRefresh( KClientNode* target );

	bool move_back( std::shared_ptr<KClientNode> targetInBucket );
	int nodeIDcmp( std::shared_ptr<KClientNode> n1 , KClientNodeVector::iterator n2 );

public:
	int autoAdd( std::shared_ptr<KClientNode> target );
	bool isFull();

	void swapForce( std::shared_ptr<KClientNode> swapFromInBucket , std::shared_ptr<KClientNode> swapTo );

	void notifyNodeSwap( std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > target );
};





};

#endif // E1BCBE66_37FA_47FD_AF31_933653808E4D

