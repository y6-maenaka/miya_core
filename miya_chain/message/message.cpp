#include "message.h"

#include "./protocol_message/block_header_msg.h"
#include "./protocol_message/block_data_msg.h"


namespace miya_chain
{



unsigned short MiyaChainMessageHeader::protocol()
{
	return static_cast<unsigned short>( _protocol );
}



bool MiyaChainMessageHeader::isRequest()
{
	return (static_cast<unsigned short>(_direction) == 1);
}




bool MiyaChainMessageHeader::importRawSequentially( std::shared_ptr<unsigned char> fromRaw )
{
	memcpy( this , fromRaw.get() , sizeof( struct MiyaChainMessageHeader ) );
	return true;
}



size_t MiyaChainMessageHeader::payloadLength()
{
	return static_cast<size_t>( ntohs(_payloadLength) );
}









MiyaChainMessage::MiyaChainMessage()
{
	if( _header == nullptr ) _header = std::make_shared<MiyaChainMessageHeader>();

	_header->_direction = static_cast<uint8_t>(1); // 送信方向
}





void MiyaChainMessage::payload( std::shared_ptr<unsigned char> target )
{
	_payload = target;
}


/*
void MiyaChainMessage::payload( void *payload , unsigned short payloadLength )
{
	_payload = payload;
	_header->_payloadLength = static_cast<uint16_t>( payloadLength );
}
*/


unsigned short MiyaChainMessage::protocol()
{
	return _header->protocol();
}



std::shared_ptr<MiyaChainMessageHeader> MiyaChainMessage::header()
{
	return _header;
}




std::shared_ptr<unsigned char> MiyaChainMessage::payload()
{

}



};
