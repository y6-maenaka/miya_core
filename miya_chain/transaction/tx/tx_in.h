#ifndef B5784F44_269B_471F_88DF_C2B666D4949D
#define B5784F44_269B_471F_88DF_C2B666D4949D



#include <stdlib.h>
#include <string.h>
#include "openssl/evp.h"



namespace tx{
class SignatureScript;


constexpr unsigned short PREV_OUT_SIZE = /*TX_ID_SIZE*/20 + (32 / 8);


struct PrevOut{

	unsigned char *_txID;
	uint32_t _index;


	/* methods */
	PrevOut();
	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();

} __attribute__((__packed__));






struct TxIn{

	/* body */
	struct PrevOut *_prevOut;
	uint32_t _script_bytes; // empty時は空
	SignatureScript *_signatureScript; // emptyは空 // coinbaseのときはここが'coinbase'欄になる
	uint32_t _sequence;


	EVP_PKEY *_pkey; // 署名と公開鍵セットに使われる
									 /* 適宜signature_scriptに鍵をセットしながら処理を行う　*/

	/* 部分署名によってそれぞれセットされる　最終的にexportされる時に格納 */
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

	/* methods */
	unsigned int exportEmptyRaw( unsigned char **ret );
	unsigned int exportEmptyRawSize();

	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();


};




};


#endif // B5784F44_269B_471F_88DF_C2B666D4949D
