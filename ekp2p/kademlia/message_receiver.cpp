#include "message_receiver.h"

#include "../network/header/header.h"

namespace ekp2p
{




void MessageReceiver::start()
{

}




unsigned int MessageReceiver::payload( void *rawEKP2PMSG ,unsigned char** ret )
{
	EKP2PMessageHeader header;
	memcpy( &(header._meta) , rawEKP2PMSG , sizeof(header._meta) );

	bool flag = header.validate();

	std::cout << "+++++++++++++" << "\n";
	std::cout << header.headerLength() << "\n";
	std::cout << header.payloadLength() << "\n";

	for( int i=0; i<header.payloadLength() + header.headerLength(); i++ )
	{
		printf("%02X", static_cast<unsigned char*>(rawEKP2PMSG)[i] );
	}std::cout << "\n";
	std::cout << "+++++++++++++" << "\n";

	std::cout << "flag " << flag << "\n";
	if( flag )
	{
		if( header.payloadLength() <= 0 ) return 0;

		*ret = new unsigned char[ header.payloadLength() ];
		std::cout << "header length -> " << header.headerLength() << "\n";
		std::cout << "payload length -> " << header.payloadLength() << "\n";
		memcpy( *ret , static_cast<unsigned char*>(rawEKP2PMSG) + header.headerLength(), header.payloadLength() );
		
		return header.payloadLength();
	}

	return 0;
};





};
