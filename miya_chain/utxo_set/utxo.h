#ifndef CF23F340_0B6A_4BCD_8BDE_554206C617DC
#define CF23F340_0B6A_4BCD_8BDE_554206C617DC


#include <memory>
#include <vector>





namespace tx
{
	struct P2PKH;
	struct Coinbase;
	struct TxOut;
}






namespace miya_chain
{


struct UTXO
{
	std::shared_ptr<unsigned char> _txID;
	unsigned short _outputIndex;

	unsigned int _amount;
	//std::shared_ptr<unsigned char> _address; // 受信者のアドレス
	// script		
	
	bool _isUsed;


protected:
	std::vector<uint8_t> dumpToBson();
	bool parseFromBson( std::vector<uint8_t> from );

public:
	void set( std::shared_ptr<tx::TxOut> target );
	
};




};


#endif // CF23F340_0B6A_4BCD_8BDE_554206C617DC



