#ifndef F68D2B16_A2C5_4CF8_85CE_D3EE525C19EA
#define F68D2B16_A2C5_4CF8_85CE_D3EE525C19EA


#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <chrono>
#include <array>

#include <string.h>
#ifdef __linux__
	#include <endian.h>
#endif
#ifdef __APPLE__
	#include <machine/endian.h>
#endif


#include <chain/transaction/coinbase/coinbase.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/utxo_set/utxo_set.h>
#include <chain/mining/simple_mining.h>

#include <hash/sha_hash.h>


namespace tx
{
	struct P2PKH;
	struct Coinbase;
}


namespace chain
{
	class LightUTXOSet;
};


namespace chain
{

/*
 nBit について
 前2bytesが指数部 ( 00000000 00000000 )
 後ろ2bytseがマントリップ ( 00000000 00000000 ) // 0の個数
 */

constexpr unsigned int BLOCK_HEADER_HASH_BITS_LENGTH = 256;
constexpr unsigned int MERKLE_ROOT_BITS_LENGTH = 256;


struct BlockHeader
{
public:

private:
	int32_t _version; //
	unsigned char _previousBlockHeaderHash[32];
	unsigned char _merkleRoot[32];
	uint32_t _time; // このブロックが生成された時のタイムスタンプ
	uint32_t _nBits; // 採掘難易度 目標のnonce値
	uint32_t _nonce; // ナンス値

public:
	BlockHeader();
	void merkleRoot( std::shared_ptr<unsigned char> target );
	void updateTime();
	uint32_t nonce();

	size_t importRawSequentially( std::shared_ptr<unsigned char> fromRaw );
	size_t importRawSequentially( void *fromRaw );
	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );

	uint32_t time();
	uint32_t nBits();
	void nBits( uint32_t target );
	void nonce( uint32_t target );
	void prevBlockHash( std::shared_ptr<unsigned char> target );
	std::shared_ptr<unsigned char> prevBlockHash();

	bool verify();
	bool compare( std::shared_ptr<unsigned char> targetBlockHash );

	size_t headerHash( std::shared_ptr<unsigned char> *ret );
	std::shared_ptr<unsigned char> headerHash();
	void print();

    bool cmpMerkleRoot( std::shared_ptr<unsigned char> target ); // ヘッダに格納されているマークルルートとの比較メソッド
	bool cmpPrevBlockHash( std::shared_ptr<unsigned char> target );

};


class block_header
{
public:
  using block_header_hash = std::array<std::uint8_t, BLOCK_HEADER_HASH_BITS_LENGTH/8>;
  using merkle_root = std::array<std::uint8_t, MERKLE_ROOT_BITS_LENGTH/8>;

private:
  struct meta
  {
	std::int32_t _version;
	block_header::block_header_hash _prev_block_hash;
	block_header::merkle_root _merkl_root;
	std::uint32_t _time;
	std::uint32_t _n_bits;
	std::uint32_t _nonce;
  } _meta;

};




};









#endif // F68D2B16_A2C5_4CF8_85CE_D3EE525C19EA
