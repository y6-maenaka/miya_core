#include "getdata.hpp"


namespace chain
{


void MiyaCoreMSG_GETDATA::inv( struct MiyaCoreMSG_INV target )
{
	_inv = target;
}

void MiyaCoreMSG_GETDATA::__print()
{
	_inv.__print();
}


};
