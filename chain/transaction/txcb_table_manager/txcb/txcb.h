#ifndef E5CB3339_3DDB_4BB8_8A7F_42F1FB9AF893
#define E5CB3339_3DDB_4BB8_8A7F_42F1FB9AF893


/* TxCB ( Tx Control Block ) */


#include <iostream>
#include <chrono>


namespace tx{

class P2PKH;


struct TxCB
{
private:
	
	struct {
		unsigned char *_txID;
		int _status; // ( blocked , free )
		//unsigned int timeStump;
		std::time_t _timestamp;
	} _meta;


	P2PKH *_tx;


	struct{
		TxCB *_leastChain;
		TxCB *_next;
		TxCB *_prev;
	} _control;
	/* Table 関係 */

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



