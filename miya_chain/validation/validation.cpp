#include "validation.h"

#include "../utxo_set/utxo.h"
#include "../utxo_set/utxo_set.h"
#include "../block/block.h"
#include "../transaction/tx/tx_in.h"
#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/script/script_validator.h"
#include "../mining/simple_mining.h"

namespace miya_chain
{


bool BlockValidation::verifyBlockHeader( block::BlockHeader *target )
{
    uint32_t nonce = target->nonce();

	std::shared_ptr<unsigned char> exportedRawHeader; size_t exportedRawHeaderLength;
	exportedRawHeaderLength = target->exportRaw( &exportedRawHeader );

	std::array<uint8_t,32> nBitMask = generatenBitMask(target->nBits());
	std::shared_ptr<unsigned char> rawNBitMask = std::shared_ptr<unsigned char>( new unsigned char [32] );
	std::copy( nBitMask.begin() , nBitMask.end() , rawNBitMask.get() );


	std::shared_ptr<unsigned char> ret; 
	hash::SHAHash( exportedRawHeader , exportedRawHeaderLength, &ret , "sha256" );

	
	std::cout << "------------------------------------"	 << "\n";
	std::cout << "hash :: ";
	for( int i=0; i<32; i++)
	{
		printf("%02X", ret.get()[i]);
	} std::cout << "\n";

	std::cout << "nbitmask :: ";
	for( int i=0; i<32; i++ )
	{
		printf("%02X", rawNBitMask.get()[i]);
	} std::cout << "\n";
	std::cout << "------------------------------------"	 << "\n";


	return mbitcmp( rawNBitMask.get() , ret.get() );
}







bool BlockValidation::verifyP2PKH( std::shared_ptr<tx::P2PKH> target ,std::shared_ptr<LightUTXOSet> utxoSet )
{
	// 参照しているUTXOからlockingScriptを取り出し,tx_inのlockingScriptと結合する	
	// 全てのtx_inに対して行われる

	int i=0;
	std::shared_ptr<miya_chain::UTXO> utxo;
	std::vector< std::shared_ptr<tx::TxIn> > ins = target->ins();
	for( auto itr : ins )
	{
		// ここでutxoからpk_scriptを取得する
		utxo = utxoSet->get( itr->prevOut() );

		if( utxo == nullptr ) return false;

		std::shared_ptr<unsigned char> txHash; unsigned int txHashLength;
		tx::ScriptValidator validator( utxo->pkScript(), itr->signatureScript() );

		txHashLength = target->txHashOnTxIn( i , &txHash ); // 指定のインデックスにpubKeyHashをセットして書き出す
		bool flag = validator.verifyP2PKHScript( txHash, txHashLength );
		std::cout << "単体TxIn検証 :: " << flag << "\n";
		i++;
	}
	return true;

}


};