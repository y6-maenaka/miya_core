#ifndef C0332A34_B701_4684_A255_FBE8F822ECC0
#define C0332A34_B701_4684_A255_FBE8F822ECC0

#include "txcb_table/txcb_table.h"

namespace tx{





/* AutoScaleMultipleHashTable */

// 本格的にトランザクションを管理するのはUTXO-SET , TxCBTableManager はキャッシュ的な役割を果たす　

class TxCBTableManager
{

private:
	TxCBTable _rootTable;

public:
	TxCBTableManager();

	// TxCBTableにも全く同じメソッドがある　スマートではない
	bool add( P2PKH *tx );
	P2PKH* find( unsigned char* txID );

	static unsigned int convertSymbol( unsigned char symbol );
	static unsigned char getSymbol( unsigned char *id , unsigned int layer  = 0 );
	static bool txcmp( unsigned char* txID_1, unsigned char* txID_2 );

};




}; // close tx namespace


#endif // C0332A34_B701_4684_A255_FBE8F822ECC0

