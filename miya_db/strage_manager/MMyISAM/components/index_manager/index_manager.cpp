#include "index_manager.h"


namespace miya_db{

void IndexLogicAddr::operator	+ (int offset){

	unsigned char Coffset[5]; memset( Coffset , 0x00 , sizeof(Coffset) );

	Coffset |= (Coffset[0] << 32); 
	Coffset |= (Coffset[1] << (32 - 8));
	Coffset |= (Coffset[2] << (32 - 16));
	Coffset |= (Coffset[3] << (32 - 24));
	Coffset |= (Coffset[4] << (32 - 32));

}



}; // close miya_db namespace
