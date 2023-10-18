// #include "../message/message.h"

#include "./stun_message.h"
#include "../inband/inband_manager.h"

namespace ekp2p
{




int StunMessageHandler( void* arg , EKP2PMSG *msg )
{
	/*
	BaseInbandManager *inbandManager = (BaseInbandManager *)arg; // inbandManagerの取り出し
	sockaddr_in* globalAddr = (sockaddr_in*)inbandManager->handlerArg(); // inbadManagerからハンドラ引数の取り出し

	if( msg == nullptr ){
		globalAddr = nullptr;
		return -1;
	}

	StunResponse *response = new StunResponse;
	response = (StunResponse *)msg->payload();

	if( response->messageType() != BINDING_RESPONSE ){ // 受信したメッセージがstunのものではない
		delete response;
		return 0;
	}

	//response->sockaddr( globalAddr ); // globalAddr の取得
	response->sockaddr( globalAddr );
	//*globalAddr = *(response->sockaddr());

	delete response;
	return 1;
	*/
}




}; // close ekp2p namespace 
