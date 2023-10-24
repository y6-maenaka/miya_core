#include "stun_message.h"


namespace ekp2p{



size_t StunRequest::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct StunRequest)] );
	memcpy( (*retRaw).get(), this , sizeof(struct StunResponse) );
}




void StunResponse::sockaddr( sockaddr_in *ret )
{

	ret->sin_addr.s_addr = _requesterAddr._ipv4;
	ret->sin_port = _requesterAddr._port;

	//return &_requesterAddr;
}


bool StunResponse::validate()
{
	return false;
}


unsigned int StunResponse::importRaw( unsigned char *from , unsigned int fromSize )
{
	memcpy( this , from , sizeof( struct StunResponse ) );
	return sizeof( struct StunResponse );
}


unsigned int StunResponse::exportRaw( unsigned char **ret )
{
	*ret = new unsigned char[ sizeof( struct StunResponse) ];

	unsigned int currentPtr = 0;

	memcpy( *ret , &_message_type , sizeof(_message_type) ); currentPtr += sizeof( _message_type );
	memcpy( *ret + currentPtr , &_message_size , sizeof(_message_size) ); currentPtr += sizeof( _message_size );
	memcpy( *ret + currentPtr , &_requesterAddr , sizeof(_requesterAddr) ); currentPtr += sizeof( _requesterAddr );

	return currentPtr;
}



};
