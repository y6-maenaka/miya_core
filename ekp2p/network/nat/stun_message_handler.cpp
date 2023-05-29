#include "../message/message.h"

#include "./stun_message.h"
#include "../inband/inband_manager.h"

namespace ekp2p
{




int StunMessageHandler( void* arg , EKP2PMSG *msg )
{
	if( msg == nullptr ) return -1;

	BaseInbandManager *inbandManager = (BaseInbandManager *)arg;

	sockaddr_in* globalAddr = (sockaddr_in*)inbandManager->handlerArg();

	StunResponse *response = new StunResponse;
	response = (StunResponse *)msg->payload();

	response->sockaddr( globalAddr ); // globalAddr の取得

	delete response;
	return 0;
}




}; // close ekp2p namespace 
