#include "getdata.h"







namespace miya_chain
{


void MiyaChainMSG_GETDATA::inv( struct MiyaChainMSG_INV target )
{
	_inv = target;
}

void MiyaChainMSG_GETDATA::__print()
{
	_inv.__print();
}



};
