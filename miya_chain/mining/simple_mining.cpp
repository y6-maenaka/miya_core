//#include "simple_mining.h"
#include "./simple_mining.h"


#include "../block/block.h"
#include "../../shared_components/hash/sha_hash.h"


namespace miay_chain
{



/*
uint32_t simpleMining( uint32_t nBits , std::shared_ptr<block::Block> block )
{


	unsigned char ucnBits[4]; memcpy( ucnBits, &nBits, sizeof(ucnBits) );
	uint16_t coefficient; // 係数部 
	memcpy( &coefficient , ucnBits, sizeof(coefficient) );

	uint16_t exponent; // 指数部 
	memcpy( &exponent , ucnBits + 2 , sizeof(exponent) );
	

	//std::shared_ptr<unsigned char> nBitsMask = std::shared_ptr<unsigned char>( new unsigned char[32] );
	std::array<uint8_t, 32 > nBitsMask; nBitsMask.fill(0x00);
	std::fill( nBitsMask.begin() , nBitsMask.begin() + static_cast<int>(coefficient), 0xff );


	for( auto itr : nBitsMask )
	{
		printf("%02X", itr );
	} std::cout << "\n";

}
*/




bool mbitcmp( unsigned char *nonceMask , std::shared_ptr<unsigned char> hash )
{
	unsigned char temp;
	for( int i=0; i<32; i++ )
	{
		temp = nonceMask[i] & ~(hash.get()[i]);
		if( temp == 0x00 ) return true;
		if( temp == nonceMask[i] ) continue;
		else return false;
	}
	return true;
}






};
