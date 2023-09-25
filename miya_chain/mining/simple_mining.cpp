#include "simple_mining.h"


#include "../block/block.h"
#include "../../shared_components/hash/sha_hash.h"


namespace miay_chain
{




uint32_t simpleMining( uint32_t nBits , std::shared_ptr<block::Block> block )
{

	uint32_t nonce = block->header().nonce();
	std::shared_ptr<unsigned char> rawHeader; unsigned int rawHeaderLength;
	block->header().updateTime();

	rawHeaderLength = block->exportHeader( &rawHeader );

	unsigned char nonceMask[32];


	auto createNonceMack = [&](uint32_t nonce) -> std::shared_ptr<unsigned char> {
		    // 関数の本体
				//     return nullptr;
				//     };
	
		std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[32] );
		memset( ret.get() , 0x00 , 32 );

		uint16_t temp; memcpy( &temp, &nonce, 8 );

		return ret;
};

	



	std::shared_ptr<unsigned char> hashedHeader;
	size_t noncePtrInRawHeader = sizeof( struct block::BlockHeader ) - sizeof(struct block::BlockHeader) - sizeof(uint32_t);
	for(;;)	
	{
		hash::SHAHash( rawHeader , rawHeaderLength , &hashedHeader , "sha256" );

		if( miya_chain::mbitcmp( nonceMask , hashedHeader ) ) break;


		//memset( rawHeader.get() + sizeof());
		// nonceを進めた値を書き込む
		memcpy( rawHeader.get() + noncePtrInRawHeader , &nonce , sizeof(uint32_t));
	}
	

	return nonce;
}



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
