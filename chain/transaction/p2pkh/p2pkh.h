#ifndef C7DC3CB1_3E91_4999_B4AC_A1D44A69A420
#define C7DC3CB1_3E91_4999_B4AC_A1D44A69A420


#include <stdlib.h>
#include <vector>
#include <memory>


#include <chain/transaction/tx/tx_in.h>
#include <chain/transaction/tx/tx_out.h>
#include <chain/transaction/script/script.h>
#include <chain/transaction/script/script_validator.h>
#include <chain/transaction/script/signature_script.h>
#include <chain/transaction/script/pk_script.h>
#include <cipher/ecdsa_manager.h>
#include <hash/sha_hash.h>

#include <chain/utxo_set/utxo.h>
#include <chain/utxo_set/utxo_set.h>
#include <chain/validation/validation.h>

#include "openssl/bio.h" 
#include "openssl/evp.h"

#include <chain/transaction/tx.params.hpp>


//#include "../tx/tx_in.h"
//#include "../tx/tx_out.h"

namespace chain
{
	class LightUTXOSet;
}


namespace tx{


struct TxIn;
struct TxOut;


struct P2PKH : public std::enable_shared_from_this<P2PKH>
{
private:
	struct
	{
		int32_t _version;
		std::vector< std::shared_ptr<TxIn> > _ins;
		std::vector< std::shared_ptr<TxOut> > _outs;
	} _meta;

	unsigned int formatExportedRawTxVector( std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxVector, std::shared_ptr<unsigned char> *retRaw ); 

public:
	P2PKH();

	std::shared_ptr<unsigned char> txID(); // このトランザクションのtxIDを計算する

	unsigned short inCount();
	unsigned short outCount();
	
	int add( std::shared_ptr<TxIn> target );
	int add( std::shared_ptr<TxOut> target );

	std::vector< std::shared_ptr<TxIn> > ins();
	std::vector< std::shared_ptr<TxOut> > outs();
	
	bool sign(); // 全てのtxに対して署名を行う
	bool verify( std::shared_ptr<chain::LightUTXOSet> utxoSet /* prevOutの問い合わせ用オブジェクト */ ); 

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw ); // 書き出しに先駆けて必ず署名を完了させる必要がある
	size_t importRawSequentially( std::shared_ptr<unsigned char> fromRaw );
	size_t importRawSequentially( void* fromRaw );

	unsigned int txHashOnTxIn( int index , std::shared_ptr<unsigned char> *retRaw );

	unsigned int calcTxID( std::shared_ptr<unsigned char> *ret ); // 一旦全てを書き出してハッシュするため頻繁には呼び出さないこと
	/*
	必要な機能とデータ
	-> txID(); 
	-> 各tx_inに対しての署名作成
	*/
  
  std::vector<std::uint8_t> export_to_binary(); // とりあえず既存exportRawのwrraperで 後で本実装する
  tx_id get_id() const;
};


};


/*
 
	int32_t version
	int16_t in_cnt
	  
		tx_in

	int16_t out_cnt

		tx_out

	



 
*/


#endif // C7DC3CB1_3E91_4999_B4AC_A1D44A69A420

