#ifndef AA02E17E_A463_4D94_898B_15D4B6598B83
#define AA02E17E_A463_4D94_898B_15D4B6598B83


#include <iostream>
#include <memory>


#include "../../../block/block.h"
#include "../../../../shared_components/miya_db_client/miya_db_sb_client.h"



class SBSegment;
class StreamBuffer;
class StreamBufferContainer;




namespace tx{
	struct P2PKH;
}




namespace miya_chain
{


class TransactionStore : MiyaDBSBClient
{
public:
	TransactionStore( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer );


	std::shared_ptr<tx::P2PKH> get( std::shared_ptr<unsigned char> txID );
	bool add( std::shared_ptr<tx::P2PKH> targetTx );
	

};





};


#endif // 

