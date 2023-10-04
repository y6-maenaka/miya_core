#ifndef FEBF8752_FAA3_4C86_B045_1BC8265E9E9E
#define FEBF8752_FAA3_4C86_B045_1BC8265E9E9E




#include <memory>
#include <vector>
#include <unistd.h>




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
}




namespace miya_chain
{


struct UTXO;



class LightUTXOSet
{
private:
	std::shared_ptr<StreamBufferContainer> _pushSBContainer;
	std::shared_ptr<StreamBufferContainer> _popSBContainer;


public:
	LightUTXOSet( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer );


	std::shared_ptr<UTXO> find( std::shared_ptr<unsigned char> txID , unsigned short index );
	void store( std::shared_ptr<tx::P2PKH> targetTx ); // 一応dumpしてSBCに流す


	void testInquire();

};







};




#endif // FEBF8752_FAA3_4C86_B045_1BC8265E9E9E






