#ifndef D5D55F80_65CA_4AB1_B528_F6FE00A52102
#define D5D55F80_65CA_4AB1_B528_F6FE00A52102




#include <iostream>
#include <memory>

#include "../../../block/block.h"
#include "../../../../shared_components/miya_db_client/miya_db_sb_client.h"


class SBSegment;
class StreamBuffer;
class StreamBufferContainer;



namespace miya_chain
{




class HeaderStore : protected MiyaDBSBClient
{

public:
	HeaderStore( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer );


	std::shared_ptr<block::BlockHeader> get( std::shared_ptr<unsigned char> blockHash );
	bool add( std::shared_ptr<block::Block> block );
	bool add( block::BlockHeader* blockHeader );

};





};




#endif // D5D55F80_65CA_4AB1_B528_F6FE00A52102



