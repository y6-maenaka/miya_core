#include "txcb.h"


namespace chain
{


TxCB::TxCB( std::shared_ptr<tx::P2PKH> target , int status )
{
	if( target == nullptr ) return;
	_tx = target;
	_meta._status = status;

	
	target->calcTxID( &(_meta._txID) );
}



/*
TxCB::TxCB( std::shared_ptr<unsigned char> txID ,int status )
{
	_meta._txID = txID;
	_meta._status = status;
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

void TxCB::txID( std::shared_ptr<unsigned char> target )
{
	_meta._txID = target;
}
	
int TxCB::priority()
{
	return _control._priority;
}

void TxCB::priority( int priority )
{
	_control._priority = priority;
}


};
