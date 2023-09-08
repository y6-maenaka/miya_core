#include "message_broker.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"


namespace miya_chain
{



void MiyaChainMessageBrocker::start()
{

	if( _sourceSBC == nullptr ) return;

	std::unique_ptr<SBSegment> popedSB;
	MiyaChainMessageHeader header;



	auto routeMiyaChainMSG = [&]( unsigned short protocol )
	{
		switch( protocol )
		{
			case MIYA_CHAIN_PROTOCOL_BLOCK_HEADER:
				break;
			case MIYA_CHAIN_PROTOCOL_BLOCK_DATA:
				break;
			case MIYA_CHAIN_PROTOCOL_TX:
				break;
			default:
				break;
		}
	};



	for(;;)
	{
		popedSB = _sourceSBC->popOne();
		memcpy( &header , (popedSB->body()).get() , sizeof( struct MiyaChainMessageHeader ) );

		if( header.isRequest() ) // リクエスト以外のメッセージは破棄する
			routeMiyaChainMSG( header.protocol() ); // メッセージプロトコルに応じて行き先を振り分ける
	}


};










void MiyaChainMessageBrocker::setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination )
{
	printf("StreamBuffer seted with -> %p\n", target.get() );
	_sbHub.at(destination) = target;
}








}
