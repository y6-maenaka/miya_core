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


	/*
	 トランザクションの検証と中継
	 [ 検証する内容 ]
	 1. トランザクションの内容に不正はないか
	 2. 支払いに使用される入力は正常か
	 3. 既に見たトランザクションの中継はしない
	 */



	for(;;)	
	{
		popedSB = _sourceSBC->popOne(); // ストリームバッファから要素を取り出す
		
		if( popedSB->body() == nullptr || popedSB->bodyLength() <= 0 ) continue;

		//targetTx = std::make_shared<tx::P2PKH>( popedSB->body().get() , popedSB->bodyLength() ); // ストリームバッファ要素からトランザクションを取り出す -> P2PKHに変換する

		// 第1検証　：　トランザクション本体の検証
		//if( targetTx->verify() ) // トランザクション自体に不整合がないかチェック　正規の秘密鍵で署名されたものか？　等々
			//_txPool->store( targetTx );

		// 第２検証　：　トランザクション入出力の検証
		// ViewTxPoolを使用する	
		// 出力使用フラグの使用 // トランザクションのどの出力が使われているかを把握する必要がある
		
		auto relay = ([&](){ // 他ノードに伝搬する

		});
	}
	

}




};
