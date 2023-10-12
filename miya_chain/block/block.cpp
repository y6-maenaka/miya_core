#include "block.h"

#include "../transaction/coinbase/coinbase.h"
#include "../transaction/p2pkh/p2pkh.h"
#include "../../shared_components/hash/sha_hash.h"

namespace block
{




BlockHeader::BlockHeader()
{
	_version = htonl(0);
	_time = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}


void BlockHeader::merkleRoot( std::shared_ptr<unsigned char> target )
{
	memcpy( _merkleRoot , target.get() , 32 );
}



void BlockHeader::updateTime()
{	
	_time = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}


unsigned int BlockHeader::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct BlockHeader)] );
	memcpy( (*retRaw).get(), this, sizeof(struct BlockHeader) );

	return sizeof(struct BlockHeader);
}



uint32_t BlockHeader::nonce()
{
	return _nonce;
}



uint32_t BlockHeader::time()
{
	return _time;
}


uint32_t BlockHeader::nBits()
{
	return _nBits;
}


void BlockHeader::nBits( uint32_t target )
{
	_nBits = target;
}


void BlockHeader::nonce( uint32_t target )
{
	_nonce = target;
}




void BlockHeader::print()
{
	std::cout << "-------------------------------------" << "\n";
	std::cout << "[ Block Header Information ]" << "\n";
	std::cout << "Version :: " << _version << "\n";

	std::cout << "PrevBlockID :: ";
	for( int i=0; i<sizeof(_previousBlockHeaderHash); i++ ){
		printf("%02X", _previousBlockHeaderHash[i] );
	} std::cout << "\n";


	std::cout << "MerkleRoot :: ";
	for( int i=0; i<sizeof(_merkleRoot); i++ ){
		printf("%02X", _merkleRoot[i]);
	} std::cout << "\n";

	std::cout << "time :: "  << _time << "\n";
	std::cout << "nBtis :: " << _nBits << "\n";
	std::cout << "nonce :: " << _nonce << "\n";
	std::cout << "-------------------------------------" << "\n";
}






Block::Block()
{
	_coinbase = nullptr;
}



void Block::header( std::shared_ptr<BlockHeader> target )
{
	_header = *target;
}



void Block::header( BlockHeader target )
{
	_header = target;
}



std::vector< std::shared_ptr<tx::P2PKH> > Block::txVector()
{
	return _txVector;
}


void Block::coinbase( std::shared_ptr<tx::Coinbase> coinbase )
{
	_coinbase = coinbase;
}



void Block::add( std::shared_ptr<tx::P2PKH> p2pkh )
{
	_txVector.push_back( p2pkh );
}




unsigned int Block::calcMerkleRoot( std::shared_ptr<unsigned char> *ret )
{

	std::vector< std::shared_ptr<unsigned char> > txIDVector;
	std::shared_ptr<unsigned char> txID; unsigned int txIDLength;

	if( _coinbase == nullptr ){
		std::cout << "コインベーストランザクションがセットされていません" << "\n";
		return 0;
	}
	txIDLength = _coinbase->calcTxID( &txID ); // コインベーストランザクションIDの算出と追加
	txIDVector.push_back( txID );


	for( auto itr : _txVector ) // 通常トランザクションのトランザクションIDの算出と追加
	{
		txIDLength = itr->calcTxID( &txID );
		txIDVector.push_back( txID );
	}



	if( txIDVector.size() % 2 != 0 ){ // ID要素が奇数個の場合は最後尾の要素を複製する
		txIDVector.push_back( txIDVector.back() );
	}


	std::function<std::vector<std::shared_ptr<unsigned char>>(std::vector<std::shared_ptr<unsigned char>>)> calcRoot = [&]( std::vector<std::shared_ptr<unsigned char>> txIDVector ){
		if( txIDVector.size() <= 1 ) return txIDVector;
		
		std::vector< std::shared_ptr<unsigned char>> altTxVector; // 代替ベクター
		std::shared_ptr<unsigned char> joinedTxID = std::shared_ptr<unsigned char>( new unsigned char[32*2] );
		std::shared_ptr<unsigned char> hashedJoinedTxID; unsigned int hashedJoinedTxIDLength;

		for( int i=0; i<txIDVector.size() ; i+=2 )
		{
			memcpy( joinedTxID.get(), txIDVector.at(i).get() , 32 );
			memcpy( joinedTxID.get() + 32 , txIDVector.at(i).get() , 32 );
			hashedJoinedTxIDLength = hash::SHAHash( joinedTxID, 32 * 2 , &hashedJoinedTxID , "sha256" );

			altTxVector.push_back( hashedJoinedTxID );
		}

		return calcRoot( altTxVector );
	};


	std::vector< std::shared_ptr<unsigned char> > retVector;
	retVector = calcRoot( txIDVector );

	if( retVector.empty() || retVector.size() != 1 ) return 0;

	*ret = std::shared_ptr<unsigned char>( new unsigned char[32] );
	*ret = retVector.front();

	return 32;
}




void Block::merkleRoot( std::shared_ptr<unsigned char> target )
{
	return _header.merkleRoot( target );
}


unsigned int Block::exportHeader( std::shared_ptr<unsigned char> *retRaw )
{
	return _header.exportRaw( retRaw );
}


uint32_t Block::time()
{
	return _header.time();
}




};
