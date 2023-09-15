#include "txcb.h"


#include "../transaction/tx/tx.h"

namespace miya_chain
{



/*
TxCB::TxCB( std::shared_ptr<tx::Tx> target )
{
	_tx = target;
}
*/







std::shared_ptr<TxCB> TxCB::prev()
{
	return _control._prevCB;
}



std::shared_ptr<TxCB> TxCB::next()
{
	return _control._nextCB;
}








void TxCB::prev( std::shared_ptr<TxCB> target )
{
	_control._prevCB = target;
}



void TxCB::next( std::shared_ptr<TxCB> target )
{
	_control._nextCB = target;
}





std::shared_ptr<tx::P2PKH> TxCB::tx()
{
	return _tx;
}



std::shared_ptr<unsigned char> TxCB::txID()
{
	return _meta._txID;
}


};
