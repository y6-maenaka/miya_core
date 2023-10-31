#ifndef A09A9587_2658_4CEC_B975_45186E97B9C3
#define A09A9587_2658_4CEC_B975_45186E97B9C3




#include <memory>
#include <iostream>




struct SBStream;
class StreamBufferContainer;



namespace block
{
	struct Block;
	struct BlockHeader;
}

namespace tx
{
	struct P2PKH;
}


namespace miya_chain
{


class LightUTXOSet;



class BlockValidation
{
public:
	static bool verifyBlockHeader( block::BlockHeader *target );
	static bool verifyP2PKH( std::shared_ptr<tx::P2PKH> target ,std::shared_ptr<LightUTXOSet> utxoSet );
};






};






#endif // A09A9587_2658_4CEC_B975_45186E97B9C3



