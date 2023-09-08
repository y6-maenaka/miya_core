#include "block_data_msg.h"



namespace miya_chain
{




void BlockDataRequestMessage::blockHash( std::shared_ptr<unsigned char> from )
{
	memcpy( _blockHash, from.get() , sizeof(_blockHash) );
}



unsigned short BlockDataRequestMessage::exportRaw( std::shared_ptr<unsigned char> ret )
{
	if( ret == nullptr ) ret = std::make_shared<unsigned char>(sizeof( struct BlockDataRequestMessage ));

	memcpy( ret.get() , this , sizeof( struct BlockDataRequestMessage) );

	return sizeof(struct BlockDataRequestMessage);
}




};
