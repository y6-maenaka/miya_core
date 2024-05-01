#ifndef F4666B08_7795_43DE_89AA_6D147306F4CA
#define F4666B08_7795_43DE_89AA_6D147306F4CA


#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <chrono>
#include <array>

#include <chain/block/block_header.h>
#include <chain/transaction/coinbase/coinbase.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/utxo_set/utxo_set.h>

#include <hash/sha_hash.h>


#include <string.h>
#ifdef __linux__
	#include <endian.h>
#endif
#ifdef __APPLE__
	#include <machine/endian.h>
#endif


namespace tx
{
  struct P2PKH;
  struct Coinbase;
}


namespace chain
{


class LightUTXOSet;


constexpr unsigned int BLOCK_HASH_BITS_LENGTH = 256;


struct Block
{
public:
  using block_hash = std::array<std::uint8_t, 160/8>;

  Block(); //
  std::shared_ptr<struct BlockHeader> header(){ return _header; };
  void header( std::shared_ptr<struct BlockHeader> target );

  std::shared_ptr<tx::Coinbase> coinbase();
  std::vector< std::shared_ptr<tx::P2PKH> > txVector();
  size_t txCount(); // coinbaseを含まない

  void coinbase( std::shared_ptr<tx::Coinbase> coinbase ); // setter
  void add( std::shared_ptr<tx::P2PKH> p2pkh ); // getter

  unsigned int calcMerkleRoot( std::shared_ptr<unsigned char> *ret );
  void merkleRoot( std::shared_ptr<unsigned char> target );

  /* ヘッダー関連 */
  unsigned int exportHeader( std::shared_ptr<unsigned char> *retRaw );
  size_t blockHash( std::shared_ptr<unsigned char> *ret );
  block_hash get_block_hash() const;
  std::shared_ptr<unsigned char> blockHash();
  //size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ); // ヘッダとトランザクション部まとめて書き出すようなことはしない

  uint32_t time();
  uint32_t height();
  bool verify( std::shared_ptr<LightUTXOSet> utxoSet );
  bool compare( std::shared_ptr<unsigned char> targetBlockHash );

  #if DEBUG
  void __printPrevBlockHash();
  void __printBlockHash();
  #endif

private:
//public:
	std::shared_ptr<BlockHeader> _header;
	std::shared_ptr<tx::Coinbase> _coinbase;
	std::vector< std::shared_ptr<tx::P2PKH> > _txVector;
};


/* struct block // 今後はこっちを使う
{
public:
  using block_hash = std::array< std::uint8_t, BLOCK_HASH_BITS_LENGTH/8>;

private:
  // class block_header _header;
  // struct tx::Coinbase _coinbase;
}; */


using block_hash = std::array<std::uint8_t, 160/8>; // 後で修正する
// using block_hash = Block::block_hash;


}; // namespace block


#endif // F4666B08_7795_43DE_89AA_6D147306F4CA
