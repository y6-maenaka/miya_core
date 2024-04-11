#ifndef F4666B08_7795_43DE_89AA_6D147306F4CA
#define F4666B08_7795_43DE_89AA_6D147306F4CA



#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <chrono>

// #include "./block_header.h"
#include <block/block_header.h>

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


namespace miya_chain
{
	class LightUTXOSet;
};


namespace block
{

struct BlockHeader;





struct Block
{
private:
//public:
	std::shared_ptr<BlockHeader> _header;
	std::shared_ptr<tx::Coinbase> _coinbase;
	std::vector< std::shared_ptr<tx::P2PKH> > _txVector;

public:
	Block(); //

	//BlockHeader header(){ return _header; };
	// BlockHeader* header(){ return &_header; };
	std::shared_ptr<BlockHeader> header(){ return _header; };
	// std::shared_ptr<BlockHeader> headerWithSharedPtr(){ return std::make_shared<BlockHeader>(_header); };
	void header( std::shared_ptr<BlockHeader> target );
	// void header( BlockHeader target );

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
	std::shared_ptr<unsigned char> blockHash();
	//size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ); // ヘッダとトランザクション部まとめて書き出すようなことはしない

	uint32_t time();
	uint32_t height();
	bool verify( std::shared_ptr<miya_chain::LightUTXOSet> utxoSet );
	bool compare( std::shared_ptr<unsigned char> targetBlockHash );

	void __printPrevBlockHash();
	void __printBlockHash();
};


};


#endif // F4666B08_7795_43DE_89AA_6D147306F4CA
