#include "../table_wrapper/table_wrapper.h"

#include "../node.h"
#include "../k_tag.h"
// #include "../k_bucket.h"




namespace ekp2p{


/*
void KRoutingTable::PONGHandler( Node *targetNode )
{

	unsigned short branch;
	KBucket *bucket;
	NodeListElem *targetElem;

	bucket = _bucketList[ (branch = targetNode->calcBranch( myNodeID() )) ];

	targetElem = bucket->nodeList()->findNode( targetNode );
	// 予約フラグを降ろす　これで候補ノードと入れ替えられることはなくなる
	targetElem->isReserved( false );
		
	return;
}
*/



} // close ekp2p namespace
