#include "k_routing_table_updator.h"

#include "./k_routing_table.h"

#include "../k_node.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

namespace ekp2p
{




KRoutingTableUpdator::KRoutingTableUpdator( KRoutingTable *target )
{
	_routingTable = target;
}




void KRoutingTableUpdator::start()
{

	if( _sourceSBC == nullptr )
		return;

	std::unique_ptr<SBSegment> popedSB;
	std::shared_ptr<KNodeAddr> targetNodeAddr;
	std::shared_ptr<KBucket> targetBucket;
	int updateFlag;
	for(;;)
	{

		popedSB =_sourceSBC->popOne();
		targetNodeAddr = popedSB->sourceKNodeAddr();

		updateFlag = _routingTable->autoAdd(  targetNodeAddr.get() );
		switch( updateFlag )
		{
			case 1:
				break;
		}
		
	// ノードが申告してきたアドレスが正しいものは判断する
	//_routingTable.find( targetNodeAddr.get() ); // ルーティングテーブル内にノードが存在するか調べる
	}

	
	return;
}




/* Setter */
void KRoutingTableUpdator::setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target )
{
	_sourceSBC = target;
}


void KRoutingTableUpdator::setSourceStreamBuffer( std::shared_ptr<StreamBufferContainer> target )
{
	_destinationSBC = target;
}









};
