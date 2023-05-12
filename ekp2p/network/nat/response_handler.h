#ifndef F71FFEDD_ADD1_4789_8DD2_E51F108275E7
#define F71FFEDD_ADD1_4789_8DD2_E51F108275E7



#include "./stun_message.h"

#include <iostream>



namespace ekp2p{

struct ReceiveThreadArgs;




class StunResponseHandler{
		
private:
	RequesterAddr *_requesterAddr;

public:
	StunResponseHandler();

	void takeInRawResponse( void* target );

	RequesterAddr* requesterAddr(); // getter
};







void stunResponseHandler( void* payload, unsigned int payloadSize ,void* free , ReceiveThreadArgs* receiveThreadArgs , struct sockaddr* peerAddr );



}; // close ekp2p namespace


#endif  // F71FFEDD_ADD1_4789_8DD2_E51F108275E7

