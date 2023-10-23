#include "message.h"

#include "../header/header.h"


namespace ekp2p
{



EKP2PMessage::EKP2PMessage()
{
	_header = std::shared_ptr<EKP2PMessageHeader>( new EKP2PMessageHeader() );
}




void EKP2PMessage::payload( std::shared_ptr<unsigned char> payload , size_t payloadLength )
{
	if( payload == nullptr || payloadLength == 0)
	{
		_header->payloadLength(0);
		_payload = nullptr;
		return;
	}

	_header->payloadLength( payloadLength );
	_payload = payload;
}




size_t EKP2PMessage::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> exportedHeader; size_t exportedHeaderLength;
	exportedHeaderLength = _header->exportRaw( &exportedHeader );

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedHeaderLength + _header->payloadLength()] );

	size_t formatPtr = 0;
	memcpy( (*retRaw).get() , exportedHeader.get(), exportedHeaderLength ); formatPtr += exportedHeaderLength;
	memcpy( (*retRaw).get() + formatPtr , _payload.get() , _header->payloadLength() ); formatPtr += _header->payloadLength();

	return formatPtr;
}



void EKP2PMessage::protocol( int type )
{
	_header->protocol( type );
}




std::shared_ptr<EKP2PMessageHeader> EKP2PMessage::header()
{
	return _header;
}



void EKP2PMessage::payload( std::shared_ptr<unsigned char> target )
{
	_payload = target;
}


std::shared_ptr<unsigned char> EKP2PMessage::payload()
{
	return _payload;
}





void EKP2PMessage::printRaw()
{
	std::cout << "\n ------------ [ Header ] ------------- " << "\n";
	_header->printRaw();

	std::cout << "\n ------------ [ Content ] ------------- " << "\n";
	std::cout << "\x1b[33m";
	for( int i=0; i<_header->payloadLength(); i++ )
	{
		printf("%02X", _payload.get()[i]);
	} std::cout << "\x1b[39m" << "\n\n";
}


};
