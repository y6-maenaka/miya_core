#include "transaction_validator.h"


#include "../../transaction/p2pkh/p2pkh.h"

#include "../../transaction_pool/transaction_pool.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"


namespace miya_chain
{






TransactionValidator::TransactionValidator( std::shared_ptr<TransactionPool> txPool , std::shared_ptr<StreamBufferContainer> sourceSBC )
{
	_txPool = txPool;
	_sourceSBC = sourceSBC;
};




void TransactionValidator::start()
{

	if( _sourceSBC == nullptr )
		return;

	std::unique_ptr<SBSegment> popedSB;
	std::shared_ptr<tx::P2PKH> targetTx;
	int updateFlag = -1;

	
	for(;;)	
	{
		popedSB = _sourceSBC->popOne(); // ストリームバッファから要素を取り出す
		
		if( popedSB->body() == nullptr || popedSB->bodyLength() <= 0 ) continue;

		targetTx = std::make_shared<tx::P2PKH>( static_cast<unsigned char*>(popedSB->body()) , popedSB->bodyLength() ); // ストリームバッファ要素からトランザクションを取り出す -> P2PKHに変換する

		if( targetTx->verify() )
			_txPool->store( targetTx );
	}
	

}




};
