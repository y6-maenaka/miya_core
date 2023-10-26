#include "block_accemble_agent.h"

//#include "../message/protocol_message/block_data_msg.h"


namespace miya_chain
{


BlockAssembleAgent::BlockAssembleAgent( std::shared_ptr<unsigned char> targetBlockHash )
{
	_requestBlockHash = targetBlockHash;
}






void BlockAssembleAgent::autoRequest()
{



}






};
