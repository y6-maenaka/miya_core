#ifndef B5784F44_269B_471F_88DF_C2B666D4949D
#define B5784F44_269B_471F_88DF_C2B666D4949D


#include <stdlib.h>
#include <memory>
#include <variant>
#include <string.h>
#include "openssl/evp.h"
#include <algorithm>
#include <cstdint>


#include "../../../shared_components/json.hpp"
using json = nlohmann::json;

namespace tx{
class SignatureScript;
struct PrevOut;


constexpr unsigned short PREV_OUT_SIZE = /*TX_ID_SIZE*/20 + (32 / 8);















// 作成時の署名には基本的に自身のPrivateKeyだけで良い // アドレスを複数持つ場合も
struct TxIn
{
private:
			
	struct Body// __attribute__((packed))
	{
		std::shared_ptr<PrevOut> _prevOut; // 36 bytes
		uint32_t _script_bytes; // unLockingScriptのバイト長 // 40
		std::shared_ptr<SignatureScript> _signatureScript; // unlockingScriptの本体
		uint32_t _sequence;

		// コンストラクタ
		Body();
	} _body;



	EVP_PKEY *_pkey; // 署名と公開鍵セットに使われる

public:
	TxIn();
	std::shared_ptr<SignatureScript> signatureScript(){ return _body._signatureScript; }; // テスト用getter 後に削除する

	void toCoinbaseInput( uint32_t height, std::shared_ptr<unsigned char> text , unsigned short textLength );
	std::shared_ptr<PrevOut> prevOut(); // getter

	unsigned int scriptBytes();
	void scriptBytes( unsigned int bytes );

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	unsigned int exportRawWithEmpty( std::shared_ptr<unsigned char> *retRaw ); // check OK
	unsigned int exportRawWithPubKeyHash( std::shared_ptr<unsigned char> *retRaw ); // ckeck OK
	unsigned int exportRawWithSignatureScript( std::shared_ptr<unsigned char> *retRaw ); // 署名値が格納されたTxInの書き出し

	void sign( std::shared_ptr<unsigned char> sign , unsigned int signLength , bool isSigned = false ); // 上位ラッパによって署名値が算出された後に格納する ただのsetter このクラスが自身で署名を行うことはできない
	bool isSigned();

	void pkey( EVP_PKEY *pkey );
	EVP_PKEY *pkey();

	int importRaw( std::shared_ptr<unsigned char> fromRaw );
	int importRaw( unsigned char *fromRaw );


	friend void to_json( json& from , const TxIn &to );
	friend void from_json( const json &from , TxIn &to );


};









/*
struct PrevOut{

	unsigned char *_txID;
	//std::shared_ptr<unsigned char> _txID;
	uint32_t _index; // 前トランザクションtx_outのどのoutかを特定する


	// methods 
	PrevOut();
	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();


} __attribute__((__packed__));






struct TxIn{

	// body 
	struct PrevOut *_prevOut;
	uint32_t _script_bytes; // empty時は空
	SignatureScript *_signatureScript; // emptyは空 // coinbaseのときはここが'coinbase'欄になる
	uint32_t _sequence;


	EVP_PKEY *_pkey; // 署名と公開鍵セットに使われる
									 // 適宜signature_scriptに鍵をセットしながら処理を行う

	// 部分署名によってそれぞれセットされる　最終的にexportされる時に格納 
	unsigned char* _sig;
	unsigned int _sigSize;


	unsigned char* sig(); // getter
	void sig( unsigned char *sig ); // setter
	//unsigned char **sig(); // setter || getter
	unsigned int sigSize(); // getter
	void sigSize( unsigned int sigSize ); // setter

	PrevOut *prevOut();
	void prevOut( unsigned char* prevOut , unsigned int prevOutSize );// prevOutSize は固定長だから第２引数は不要かも

	// fromの先頭が確実にSignatureScriptの先頭になっている必要がある , そしてfromのその後に確実にデータがないといけない // 結構無理やり処理
	unsigned int autoTakeInSignatureScript( unsigned char* from ); // 取り込んだ分
	unsigned int scriptBytes(); // getter

	TxIn();

	EVP_PKEY* pkey(); // getter
	void pkey( EVP_PKEY* pkey ); // setter

	SignatureScript *signatureScript(); // getter
	void signatureScript( SignatureScript *signatureScript );

	// methods 
	unsigned int exportEmptyRaw( unsigned char **ret );
	unsigned int exportEmptyRawSize();

	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();

	std::shared_ptr<unsigned char> inTxID();
	unsigned short inIndex();
};
*/





};


#endif // B5784F44_269B_471F_88DF_C2B666D4949D
