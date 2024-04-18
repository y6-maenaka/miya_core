#include "txcb.h"

#include "../../p2pkh/p2pkh.h"


namespace tx{


TxCB::TxCB( P2PKH *tx ) : _next(nullptr) , _prev(nullptr) , _leastChain(nullptr)
{
	_meta._txID = tx->txID(); 
	_tx = tx;
}




TxCB* TxCB::next()
{
	return _next;
}


TxCB* TxCB::prev()
{
	return _prev;
}


void TxCB::next( TxCB *txcb )
{
	_next = txcb;
}


void TxCB::prev( TxCB *txcb )
{
	_prev = txcb;
}



P2PKH *TxCB::tx()
{
	return _tx;
}



unsigned char* TxCB::txID()
{
	return _meta._txID;
}






}; // close tx namespace
