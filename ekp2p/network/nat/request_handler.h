#ifndef F0AFAB39_4277_4203_8070_9932D0C44BA2
#define F0AFAB39_4277_4203_8070_9932D0C44BA2


#include <iostream>


namespace ekp2p{

struct ReceiveThreadArgs;




void stunRequestHandler( void* payload, unsigned int payloadSize ,void* free  , ReceiveThreadArgs *receiveThreadArgs , struct sockaddr* peerAddr ){

};





/*
class StunRequestHandler{

public:

	bool Respond(){ return false;};

};
*/



}; // close ekp2p namespace



#endif // F0AFAB39_4277_4203_8070_9932D0C44BA2
