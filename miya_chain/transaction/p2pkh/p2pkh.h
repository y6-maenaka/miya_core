#ifndef C7DC3CB1_3E91_4999_B4AC_A1D44A69A420
#define C7DC3CB1_3E91_4999_B4AC_A1D44A69A420


#include <stdlib.h>
#include <vector>

//#include "../tx/tx_in.h"
//#include "../tx/tx_out.h"



namespace tx{




struct TxIn;
struct TxOut;


class P2PKH{

//private:
public:
	struct p2pkh{

		int32_t _version;
		std::vector<TxIn *> _ins;
		// int ins_cnt;  ※ vectorより取得する
		
		std::vector<TxOut *> _outs;
		// int outs_cnt; ※ vectorより取得する

	} _body;


	bool isSigned;
	bool isVerified;



// methods
	int TxInCnt(); // getter
	int TxOutCnt(); // getter

	/* methods */
	P2PKH(){;};
	P2PKH( unsigned char* rawP2PKHBuff , unsigned int rawP2PKHBuffSize );
	//unsigned char* sig;

	void addInput( TxIn *txIn );
	void addOutput( TxOut* txOut );

	
	bool sign(); // tx全体
	bool verify(); // tx全体


	unsigned int exportPartialTxIn( unsigned char **ret , int index ); // OK
	unsigned int exportPartialTxInSize( int index );                   // OK


	// すでに全体署名が正常に終了されていることを前提とした処理
	unsigned int exportTxInRaw( unsigned char **ret );
	unsigned int exportTxInRawSize();

	unsigned int exportTxOutRaw( unsigned char **ret ); // OK
	unsigned int exportTxOutRawSize();                  // OK


	unsigned int exportPartialRaw( unsigned char **ret , int partialTxInIndex ); // OK
	unsigned int exportPartialRawSize( int partialTxInIndex );                   // OK


	unsigned int exportRaw( unsigned char **ret );
	unsigned int exportRawSize();


	unsigned char* txID();
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

