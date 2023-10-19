#include "k_routing_table.h"

#include "./k_bucket.h"
#include "../k_node.h"


namespace ekp2p
{





KRoutingTable::KRoutingTable( std::shared_ptr<SocketManager> target )
{
	_hostNode = std::make_shared<KHostNode>(target);

	for( int i=0; i<_routingTable.size() ; i++ )
	{
		KBucket *initKBucket = new KBucket;
		_routingTable.at(i) = std::make_shared<KBucket>( *initKBucket );
	}

	std::cout << "new KRoutingTable just Initialized" << "\n";
	_hostNode->printInfo();

	// _hostNode = hostNode;
}







std::shared_ptr<KBucket> KRoutingTable::kBucket( unsigned short branch )
{
	if( branch >= 160 )	 return nullptr;

	return _routingTable.at(branch);
}

	


int KRoutingTable::autoAdd( KNodeAddr *target )
{
	short int targetBranch = calcBranch( target );
	
	std::shared_ptr<KBucket> targetBucket = kBucket(targetBranch);

	if( targetBucket == nullptr ) return -1;
	return targetBucket->autoAdd( target );
}





short int KRoutingTable::calcBranch( KNodeAddr *targetNodeAddr )
{
	unsigned char nodeXORDistance[20] = {};

	for( int i=0; i<20; i++)
		nodeXORDistance[i] = (targetNodeAddr->ID()[i]) ^ (_hostNode->kNodeAddr()->ID()[i]);


	int branch = 0;
	for( int i=0; i<160; i++ )
		branch += ((nodeXORDistance[i / 8] >> (i % 8)) & 1);

	return static_cast<short int>( branch );
}






};
