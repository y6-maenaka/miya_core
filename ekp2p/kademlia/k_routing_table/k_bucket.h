#ifndef E1BCBE66_37FA_47FD_AF31_933653808E4D
#define E1BCBE66_37FA_47FD_AF31_933653808E4D


#include <array>
#include <vector>
#include <mutex>






namespace ekp2p
{


constexpr unsigned short K_SIZE = 8;
struct KNodeAddr;
class KClientNode;








struct KBucket
{
private:
	//std::mutex mtx_;

	std::vector< std::shared_ptr<KClientNode> > _kBucket;
	std::vector< std::shared_ptr<KClientNode> >::iterator _referenceHead;

protected:
	KClientNode* find( KNodeAddr* target );
	int add( KClientNode* target );
	int bucketRefresh( KClientNode* target );

public:
	int autoAdd( KNodeAddr *target );
	bool isFull();

};





};

#endif // E1BCBE66_37FA_47FD_AF31_933653808E4D
