#include "block_header.hpp"
#include "block_header.h" // temp
#include <crypto_utils/crypto_utils.hpp>


using namespace cu;


namespace chain
{


BlockHeader::BlockHeader()
{
	#ifdef __linux__ // 普通に=0で良い
		_version = htobe32(0);
	#endif
	#ifdef __APPLE__
		_version = htonl(0);
	#endif
	_nBits = 0;
	_nonce = 0;

	memset( _previousBlockHeaderHash , 0x00 , sizeof(_previousBlockHeaderHash) );
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


size_t BlockHeader::importRawSequentially( std::shared_ptr<unsigned char> fromRaw )
{
	memcpy( this , fromRaw.get() , sizeof(struct BlockHeader) );
	// アクセスエラーが発生したらキャッチしてfalseをリターンする
	return sizeof(struct BlockHeader);
}

size_t BlockHeader::importRawSequentially( void *fromRaw )
{
	unsigned char* _fromRaw = static_cast<unsigned char *>(fromRaw);
	memcpy( this , _fromRaw , sizeof(struct BlockHeader) );
	// アクセスエラーが発生したらキャッチしてfalseをリターンする
	return sizeof(struct BlockHeader);
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


void BlockHeader::prevBlockHash( std::shared_ptr<unsigned char> target )
{
	if( target == nullptr ){
		memset( _previousBlockHeaderHash , 0x00 , sizeof(_previousBlockHeaderHash) );
		return;
	}
	memcpy( _previousBlockHeaderHash, target.get() , sizeof(_previousBlockHeaderHash));
}


std::shared_ptr<unsigned char> BlockHeader::prevBlockHash()
{
	std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_previousBlockHeaderHash)] );

	unsigned char addrZero[32]; memset( addrZero, 0x00 , sizeof(addrZero));
	if( memcmp( _previousBlockHeaderHash , addrZero , 32 ) == 0 ) return nullptr;

	memcpy( ret.get() , _previousBlockHeaderHash, sizeof(_previousBlockHeaderHash) );
	return ret;
}


size_t BlockHeader::headerHash( std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<unsigned char> exportedHeader;  size_t exportedHeaderLength;
	exportedHeaderLength = this->exportRaw( &exportedHeader );

	size_t hashLength;
	hashLength = hash::SHAHash( exportedHeader , exportedHeaderLength , ret , "sha256" );

	return hashLength;
}


std::shared_ptr<unsigned char> BlockHeader::headerHash()
{
	std::shared_ptr<unsigned char> ret;
	this->headerHash( &ret );
	return ret;
}


void BlockHeader::print()
{
	std::cout << "-------------------------------------" << "\n";
	std::cout << "[ Block Header Information ]" << "\n";
	std::cout << "Version :: " << _version << "\n";

	std::cout << "Header(Block) Hash :: ";
	std::shared_ptr<unsigned char> headerHash = this->headerHash();
	for( int i=0; i<32; i++ ){
		printf("%02X", headerHash.get()[i] );
	} std::cout << "\n";

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


bool BlockHeader::verify()
{
	uint32_t nonce = this->nonce();

	std::shared_ptr<unsigned char> exportedRawHeader; size_t exportedRawHeaderLength;
	exportedRawHeaderLength = this->exportRaw( &exportedRawHeader );

	std::array<uint8_t,32> nBitMask = generatenBitMask(this->nBits());
	std::shared_ptr<unsigned char> rawNBitMask = std::shared_ptr<unsigned char>( new unsigned char [32] );
	std::copy( nBitMask.begin() , nBitMask.end() , rawNBitMask.get() );


	std::shared_ptr<unsigned char> ret;
	hash::SHAHash( exportedRawHeader , exportedRawHeaderLength, &ret , "sha256" );


	/*
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
	*/

	return mbitcmp( rawNBitMask.get() , ret.get() );
}

bool BlockHeader::compare( std::shared_ptr<unsigned char> targetBlockHash  )
{
	return memcmp( this->headerHash().get(), targetBlockHash.get(), 32 ) == 0;
}


bool BlockHeader::cmpMerkleRoot( std::shared_ptr<unsigned char> target )
{
    return (memcmp( _merkleRoot , target.get(), sizeof(_merkleRoot)) == 0);
}


bool BlockHeader::cmpPrevBlockHash( std::shared_ptr<unsigned char> target )
{
	return (memcmp( _previousBlockHeaderHash , target.get() , sizeof(_previousBlockHeaderHash) ) == 0);
}













/* 改 block_header */

block_header::block_header() : 
  _target_nonce{0}
{
  std::memset( &_meta, 0x00, sizeof(_meta) );
}

void block_header::increment_nonce()
{
  _meta._nonce += 1;
}

block_header::header_hash block_header::generate_header_hash() const
{
  // std::size_t header_hash_length; 
  // std::shared_ptr<unsigned char> header_hash;

  if( auto cu_ret = sha1::hash( reinterpret_cast<const unsigned char*>(&_meta), sizeof(_meta) ); (*cu_ret).size() == (BLOCK_HASH_BYTES_LENGTH) )
  {
	block_header::header_hash ret; 
	auto md = cu_ret.to_vector< block_header::header_hash::value_type >();
	return block_header::header_hash( md );
	// std::copy( md.begin(), md.end(), ret.begin() );
	// return block_header::header_hash(ret);
  }
  else{
	block_header::header_hash ret;
	ret.fill(0x00);
	return ret;

	// std::fill( ret.begin(), ret.end(), 0xff ); // 衝突する可能性あり(限りなく低いが)
	// return ret;
  }
}

block_header::header_hash block_header::get_prev_block_hash() const
{
  return _meta._prev_block_hash;
}

std::vector<std::uint8_t> block_header::export_to_binary() const
{
  std::array< std::uint8_t, sizeof(_meta) > ret_a;
  std::memcpy( ret_a.data(), &_meta, sizeof(_meta) );

  return std::vector<std::uint8_t>( ret_a.begin(), ret_a.end() );
}

std::uint32_t block_header::get_time() const
{
  return _meta._time;
}

std::uint32_t block_header::get_nonce() const
{
  return _meta._nonce;
}

#if L1_DEBUG
void block_header::print_binary() const
{
  unsigned char temp[ sizeof(_meta) ]; std::memcpy( temp, &_meta, sizeof(_meta) );
  for( int i=0; i<sizeof(_meta); i++ ) printf("%02X", temp[i] );
  std::cout << "\n";
}
#endif


};
