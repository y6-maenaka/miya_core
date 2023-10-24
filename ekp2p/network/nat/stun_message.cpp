#include "stun_message.h"


namespace ekp2p{


void RequesterAddr::sockaddr_in( struct sockaddr_in from )
{
		_ipv4 = from.sin_addr.s_addr;
		_port = from.sin_port;
}

struct sockaddr_in RequesterAddr::toSockaddr_in()
{
	struct sockaddr_in ret;
	ret.sin_addr.s_addr = _ipv4;
	ret.sin_port = _port;

	return ret;
}






size_t StunRequest::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct StunRequest)] );
	memcpy( (*retRaw).get(), this , sizeof(struct StunResponse) );

	return sizeof( struct StunResponse );
}




void StunResponse::sockaddr_in( struct sockaddr_in from )
{
	_requesterAddr.sockaddr_in( from );
}



int StunResponse::importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	if( fromRawLength < sizeof(struct StunResponse) || fromRaw == nullptr ) return 0;
	memcpy( this , fromRaw.get() , sizeof( struct StunResponse ) );
	return sizeof(struct StunResponse);
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

size_t StunResponse::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
		(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct StunResponse)]);

		size_t formatPtr = 0;
		memcpy( (*retRaw).get() + formatPtr , &_message_type , sizeof(_message_type) ); formatPtr += sizeof(_message_type);
		memcpy( (*retRaw).get() + formatPtr , &_message_size , sizeof(_message_size) ); formatPtr += sizeof(_message_size);
		memcpy( (*retRaw).get() + formatPtr , &_requesterAddr, sizeof(_requesterAddr) ); formatPtr += sizeof(_requesterAddr);

		return formatPtr;
}



struct sockaddr_in StunResponse::toSockaddr_in()
{
	return _requesterAddr.toSockaddr_in();
}

};
