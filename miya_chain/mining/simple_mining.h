#ifndef F58F7A54_8BAB_48D3_B7BD_DB006039D446
#define F58F7A54_8BAB_48D3_B7BD_DB006039D446



#include <iostream>
#include <memory>
#include <bitset>
#include <array>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>




#include "../../shared_components/hash/sha_hash.h"


namespace block
{
	struct Block;
};



namespace miya_chain
{


bool mbitcmp( unsigned char* rawNBitMask , unsigned char* hash );



uint32_t simpleMining( uint32_t nBits , std::shared_ptr<block::Block> block )
{

	auto generatenBitMask = []( uint32_t nBits ) -> std::array<uint8_t, 32>
	{
		nBits = htonl(nBits);
		unsigned char ucnBits[4]; memcpy( ucnBits, &nBits, sizeof(ucnBits) );

		uint8_t exponent; memcpy( &exponent , &(ucnBits[0]) , sizeof(exponent) );
		unsigned char coefficient[3]; memcpy( &coefficient , ucnBits + sizeof(exponent) , sizeof(coefficient) );

		// nビットマスクの作成 txIDはこれ以下の値にする
		unsigned short shift = static_cast<unsigned short>(exponent) - 3;
		std::array<uint8_t, 32 > nBitsMask; nBitsMask.fill(0x00);
		nBitsMask[31-(shift+2)] = coefficient[0];
		nBitsMask[31-(shift+1)] = coefficient[1];
		nBitsMask[31-shift] = coefficient[2];

		return nBitsMask;
	};


	// ブロックヘッダを書き出す
	std::shared_ptr<unsigned char> rawBlockHeader; size_t rawBlockHeaderLength;
	rawBlockHeaderLength = block->exportHeader( &rawBlockHeader );

	// nBitsマスクの作成 効率化のため生に変換する
	std::array<uint8_t, 32 > nBitsMask = generatenBitMask( nBits );
	std::shared_ptr<unsigned char> rawNBitMask = std::shared_ptr<unsigned char>( new unsigned char[nBitsMask.size()] );
	std::copy( nBitsMask.begin() , nBitsMask.end(), rawNBitMask.get() );


	uint32_t _candidateNonce = 0;
	unsigned short nonceOffset = rawBlockHeaderLength - sizeof(_candidateNonce);	
	memcpy( rawBlockHeader.get() + nonceOffset , &_candidateNonce, sizeof(_candidateNonce) );

	std::shared_ptr<unsigned char> hash;

	while(true)	
	{

		hash::SHAHash( rawBlockHeader , rawBlockHeaderLength , &hash , "sha256" );

		std::cout << "(nonce) :: "  << _candidateNonce << " | ";
		/*
		std::cout << "(nBits) :: ";
		for( int i=0; i<32;i++)
		{
			printf("%02X", rawNBitMask.get()[i]);
		} std::cout << "\n";
		*/
		for( int i=0; i<32; i++)
		{
			printf("%02X", hash.get()[i]);
		} std::cout << "\n";


		if( (mbitcmp( rawNBitMask.get() , hash.get())) ) break; // nonce値が見つかった場合
		_candidateNonce++;
		memcpy( rawBlockHeader.get() + nonceOffset , &_candidateNonce, sizeof(_candidateNonce) ); // 変更したnonceの再配置
	}


	uint32_t miningCompletionTime = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << "\n";
	std::cout << " nonce :: " << _candidateNonce << "\n";

	std::cout << " nBits :: ";
	for( int i=0; i<32; i++)
	{
		printf("%02X", rawNBitMask.get()[i]);
	} std::cout << "\n"; 

	std::cout << " hash  :: ";
	for( int i=0; i<32; i++)
	{
		printf("%02X", hash.get()[i]);
	} std::cout << "\n";

	std::cout << " time  :: " << miningCompletionTime - block->time() << " [s]" << "\n";
	std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << "\n";

	return _candidateNonce;
	

}






// ハッシュ値の方が小さかったらtrueを返す
bool mbitcmp( unsigned char* rawNBitMask , unsigned char* hash )
{
	// 先頭ビットから大小比較を行う
	for( int i=0; i<32; i++ ){
		if( rawNBitMask[i] < hash[i] ) return false;
		else if( rawNBitMask[i] == hash[i] ) continue;
		else return true;
	}
}






};

#endif // 

