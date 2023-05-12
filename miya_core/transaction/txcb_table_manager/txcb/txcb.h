#ifndef E5CB3339_3DDB_4BB8_8A7F_42F1FB9AF893
#define E5CB3339_3DDB_4BB8_8A7F_42F1FB9AF893


/* TxCB ( Tx Control Block ) */


#include <iostream>


namespace tx{

class P2PKH;


struct TxCB{

private:
	P2PKH *_tx;
	unsigned char *_txID;


	int _status; // ( blocked , free )
	unsigned int timeStump;


	/* Table 関係 */
	TxCB *_leastChain;
	TxCB *_next;
	TxCB *_prev;


public:
	TxCB( P2PKH *tx );

	P2PKH *tx();
	unsigned char* txID();

	TxCB *next();
	TxCB *prev();

	void next( TxCB *txcb );
	void prev( TxCB *txcb );
};


}; // close tx namespace


#endif // E5CB3339_3DDB_4BB8_8A7F_42F1FB9AF893



