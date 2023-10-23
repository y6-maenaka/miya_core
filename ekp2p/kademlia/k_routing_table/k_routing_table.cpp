#include "k_routing_table.h"

#include "./k_bucket.h"
#include "../k_node.h"


namespace ekp2p
{





KRoutingTable::KRoutingTable( std::shared_ptr<SocketManager> target )
{
	_hostNode = std::make_shared<KHostNode>(target);

	for( int i=0; i<_bucketArray.size() ; i++ )
	{
		_bucketArray.at(i) = std::make_shared<KBucket>();
	}

	std::cout << "new KRoutingTable just Initialized" << "\n";
	_hostNode->printInfo();

	// _hostNode = hostNode;
}







std::shared_ptr<KBucket> KRoutingTable::kBucket( unsigned short branch )
{
	if( branch >= 160 )	 return nullptr;

	return _bucketArray.at(branch);
}

	



int KRoutingTable::autoAdd( std::shared_ptr<KNodeAddr> target )
{
	short int targetBranch = calcBranch( target );

	std::cout << "KRoutingTable::autoAdd branch >> " << targetBranch << "\n";

	std::shared_ptr<KClientNode> targetKNode = std::make_shared<KClientNode>(target);
	return _bucketArray.at(targetBranch)->autoAdd( targetKNode );
}




short int KRoutingTable::calcBranch( std::shared_ptr<KNodeAddr> targetNodeAddr )
{
	unsigned char nodeXORDistance[20] = {};

	for( int i=0; i<20; i++)
		nodeXORDistance[i] = (targetNodeAddr->ID()[i]) ^ (_hostNode->kNodeAddr()->ID()[i]);


	int branch = 0;
	for( int i=0; i<160; i++ )
		branch += ((nodeXORDistance[i / 8] >> (i % 8)) & 1);

	return static_cast<short int>( branch );
}




void KRoutingTable::notifyNodeSwap( std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > target )
{
	for( int i=0; i<_bucketArray.size() ; i++ )
	{
		_bucketArray.at(i)->notifyNodeSwap( target );
	}

}




const std::shared_ptr<KHostNode> KRoutingTable::hostNode()
{
	return _hostNode;
}





};
