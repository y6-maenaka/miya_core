#ifndef FEBF8752_FAA3_4C86_B045_1BC8265E9E9E
#define FEBF8752_FAA3_4C86_B045_1BC8265E9E9E




#include <memory>
#include <vector>
#include <unistd.h>
#include <random>



struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;







/*

	UTXOで取得するもの
*/




namespace tx
{
	struct P2PKH;
	struct Coinbase;
	struct TxOut;
	struct PrevOut;
}




namespace miya_chain
{


struct UTXO;

 // constexpr int MIYA_DB_QUERY_ADD = 1;
 // constexpr int MIYA_DB_QUERY_GET = 2;



class LightUTXOSet
{
private:
	std::shared_ptr<StreamBufferContainer> _pushSBContainer;
	std::shared_ptr<StreamBufferContainer> _popSBContainer;

protected:
	// static uint32_t generateQueryID();

public:
	LightUTXOSet( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer );


	std::shared_ptr<UTXO> get( std::shared_ptr<unsigned char> txID , uint32_t index );
	std::shared_ptr<UTXO> get( std::shared_ptr<tx::PrevOut> prevOut );

	bool add( std::shared_ptr<tx::TxOut> targetTxOut, std::shared_ptr<unsigned char> txID, uint32_t index );
	// bool add( std::shared_ptr<tx::P2PKH> targetTx ); // 一応dumpしてSBCに流す
	bool add( std::shared_ptr<tx::Coinbase> targetCoinbase );

	void testInquire( std::shared_ptr<unsigned char> data , size_t dataLength );

};







};




#endif // FEBF8752_FAA3_4C86_B045_1BC8265E9E9E






