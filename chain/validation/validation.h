#ifndef A09A9587_2658_4CEC_B975_45186E97B9C3
#define A09A9587_2658_4CEC_B975_45186E97B9C3


#include <memory>
#include <iostream>


#include "../utxo_set/utxo.h"
#include "../utxo_set/utxo_set.h"
#include "../block/block.h"
#include "../transaction/tx/tx_in.h"
#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/script/script_validator.h"
#include "../mining/simple_mining.h"
#include <chain/block/block.h>


struct SBStream;
class StreamBufferContainer;


namespace tx
{
	struct P2PKH;
}


namespace chain
{


class LightUTXOSet;



class BlockValidation
{
public:
	static bool verifyBlockHeader( BlockHeader *target );
	static bool verifyP2PKH( std::shared_ptr<tx::P2PKH> target ,std::shared_ptr<LightUTXOSet> utxoSet );
};


};


#endif // A09A9587_2658_4CEC_B975_45186E97B9C3
