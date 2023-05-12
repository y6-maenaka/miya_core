#include "../k_routing_table.h"

#include "../node.h"
#include "../k_tag.h"
#include "../k_bucket.h"

#include <map>
#include <vector>



namespace ekp2p{




void KRoutingTable::FIND_NODEHandler( Node* targetNode , int requestSize )
{
	/*
	 [ 重要 ]
	  ActiveKBucketListから基準を出すために、activeBucketListを出す前にupdateを実行する必要がある
	 */

	int offerSize;
	KBucket *bucket = NULL;

	update( targetNode );
	std::map<unsigned short , KBucket*> *activeKBucketList = ActiveKBucketList();
	std::multimap<unsigned short, KAddr*> *offerNodeMap = new std::multimap<unsigned short , KAddr*>;

	KTag *kTag = new KTag;

	int targetNodeCnt = requestSize * 3;

	// この方式だと取りすぎてしまう可能性がある まぁいい
	int i = 0;
	while( targetNodeCnt <= 0 || activeKBucketList->size() <= 0 )
	{
		for( auto itr = activeKBucketList->cbegin(); itr != activeKBucketList->cend() ; itr++ )
		{
			if(	itr->second->speNode(i) == NULL ){ activeKBucketList->erase( itr ); }
			else{	
				offerNodeMap->insert( std::make_pair( targetNode->calcBranch( itr->second->speNode(i)->nodeID() ) , itr->second->speNode(i)->kAddr() )  );
				targetNodeCnt--;
			}
		}
		i++;
	}


	auto it = offerNodeMap->cbegin();
	do
	{
		kTag->addKAddr( it->second );
		it++; requestSize--;
	}while( it != offerNodeMap->cend() || requestSize <= 0 );


	kTag->protocol( 5 );
	targetNode->send( NULL , 0 , kTag , targetNode->socketManager() );



	delete activeKBucketList;
	return;
};



}

