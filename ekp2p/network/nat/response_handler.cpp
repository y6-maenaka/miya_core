#include "response_handler.h"



namespace ekp2p{


StunResponseHandler::StunResponseHandler(){
	_requesterAddr = NULL;
}



void StunResponseHandler::takeInRawResponse( void* target )
{

	StunResponse *response = new StunResponse;
	response = (StunResponse *)target;

	if( response->messageType() != 0  ){
		std::cout << "messag type wrong" << "\n";
		return;
	}

	_requesterAddr = new RequesterAddr;
	_requesterAddr = &(response->_requesterAddr);

	return;
};


RequesterAddr* StunResponseHandler::requesterAddr(){
	return _requesterAddr;
};



void stunResponseHandler( void* payload, unsigned int payloadSize ,void* free , ReceiveThreadArgs *receiveThreadArgs , struct sockaddr* peerAddr ){

	std::cout << payloadSize << "\n";

	StunResponseHandler handler;
	handler.takeInRawResponse( payload );

	if( handler.requesterAddr() != NULL ){
		 pthread_exit( (void *)handler.requesterAddr() );
	}	

	std::cout << "PASSED" << "\n";



};

}; // close ekp2p namespace
