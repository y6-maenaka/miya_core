#include "stun_message.h"


namespace ekp2p{




void StunResponse::sockaddr( sockaddr_in *targetAddr )
{
	targetAddr->sin_port = htons( _requesterAddr._port );

	in_addr_t	ipAddr = *(in_addr_t*)_requesterAddr._ipv4;
	targetAddr->sin_addr.s_addr = ipAddr;
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
