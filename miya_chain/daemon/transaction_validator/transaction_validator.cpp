#include "transaction_validator.h"


#include "../../transaction/p2pkh/p2pkh.h"
#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"


namespace miya_chain
{






TransactionValidator::TransactionValidator( TransactionPool *txPool , std::shared_ptr<StreamBufferContainer> sourceSBC )
{
	_txPool = txPool;
	_sourceSBC = sourceSBC;
};




void TransactionValidator::start()
{

	if( _sourceSBC == nullptr )
		return;

	std::unique_ptr<SBSegment> popedSB;
	std::shared_ptr<P2PKH> targetTx;
	int updateFlag;

	for(;;)	
	{
		popedSB = _sourceSBC->popOne();
		targetTx = std::make_shared<P2PKH>( popedSB->body() , popedSB->bodyLength() );

		targetTx.validate();
		_txPool.add( targetTx );
	}


}




};
