#include "block_header_msg.h"



namespace miya_chain
{





unsigned char* BlockHeaderMessage::blockHash()
{
	return _blockHash;
}



unsigned short BlockHeaderMessage::txCount()
{
	return static_cast<unsigned short>(_txCount);
}



}
