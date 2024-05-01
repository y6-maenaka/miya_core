#ifndef CF23F340_0B6A_4BCD_8BDE_554206C617DC
#define CF23F340_0B6A_4BCD_8BDE_554206C617DC


#include <memory>
#include <vector>
#include <algorithm>



#include "../../share/json.hpp"


namespace tx
{
	struct P2PKH;
	struct coinbase;
	struct TxOut;
	class PkScript;
}






namespace chain
{


struct UTXO
{
	struct Content
	{
		std::shared_ptr<unsigned char> _txID;
		uint32_t _outputIndex;

		uint64_t _amount;
		std::shared_ptr<tx::PkScript> _pkScript;

		// bool _used; // UTXOの使用状態 ※削除するため不要
	} _content;

	std::shared_ptr<tx::TxOut> _txOut = nullptr;

public:
	UTXO(); // 必ずインポートが前提
	UTXO( std::shared_ptr<tx::TxOut> target , std::shared_ptr<unsigned char> txID,  uint32_t index ); // utxoを登録する場合
	std::vector<uint8_t> dumpToBson();
	size_t dumpToBson( std::shared_ptr<unsigned char> *retRaw );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

	size_t amount();
	size_t outputIndex();
	std::shared_ptr<tx::PkScript> pkScript();
	std::shared_ptr<unsigned char> txID();

	// void set( std::shared_ptr<tx::TxOut> target , std::shared_ptr<unsigned char> txID , unsigned short index );
	bool importFromBson( nlohmann::json fromBson );
	bool importFromJson( nlohmann::json fromJson );
	// size_t importRawSequentially( unsigned char *fromRaw ); // 読み込んだサイズを戻り値とする
};




};


#endif // CF23F340_0B6A_4BCD_8BDE_554206C617DC
