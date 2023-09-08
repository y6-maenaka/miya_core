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










MiyaChainMessage::MiyaChainMessage()
{
	if( _header == nullptr ) _header = std::make_shared<MiyaChainMessageHeader>();

	_header->_direction = static_cast<uint8_t>(1); // 送信方向
}






void MiyaChainMessage::payload( void *payload , unsigned short payloadLength )
{
	_payload = payload;
	_header->_payloadLength = static_cast<uint16_t>( payloadLength );
}


unsigned short MiyaChainMessage::protocol()
{
	return _header->protocol();
}




std::shared_ptr<BlockHeaderMessage> MiyaChainMessage::blockHeaderMSG()
{
	BlockHeaderMessage *msg = new BlockHeaderMessage;

	msg = static_cast<BlockHeaderMessage*>( _payload );

	return std::make_shared<BlockHeaderMessage>( *msg );
}





std::shared_ptr<BlockDataResponseMessage> MiyaChainMessage::blockDataResponseMSG()
{
	BlockDataResponseMessage *msg = new BlockDataResponseMessage;

	msg = static_cast<BlockDataResponseMessage*>( _payload );

	return std::make_shared<BlockDataResponseMessage>( *msg );
}





};
