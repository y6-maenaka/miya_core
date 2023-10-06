#ifndef CF23F340_0B6A_4BCD_8BDE_554206C617DC
#define CF23F340_0B6A_4BCD_8BDE_554206C617DC


#include <memory>
#include <vector>
#include <algorithm>



#include "../../shared_components/json.hpp"


namespace tx
{
	struct P2PKH;
	struct Coinbase;
	struct TxOut;
	class PkScript;
}






namespace miya_chain
{


struct UTXO
{

	struct Content
	{
		std::shared_ptr<unsigned char> _txID;
		uint32_t _outputIndex;

		uint64_t _amount;
		std::shared_ptr<tx::PkScript> _pkScript;

		bool _used; // UTXOの使用状態
	} _content;
	
	std::shared_ptr<tx::TxOut> _txOut;

	
	
protected:

public:
	UTXO(){}; // 必ずインポートが前提
	UTXO( std::shared_ptr<tx::TxOut> target ); // utxoを登録する場合
	std::vector<uint8_t> dumpToBson( bool isUsed = false );


	void set( std::shared_ptr<tx::TxOut> target , std::shared_ptr<unsigned char> txID , unsigned short index );
	bool set( nlohmann::json dbResponse );
	
};




};


#endif // CF23F340_0B6A_4BCD_8BDE_554206C617DC



