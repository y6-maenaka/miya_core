#ifndef F4666B08_7795_43DE_89AA_6D147306F4CA
#define F4666B08_7795_43DE_89AA_6D147306F4CA



#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <chrono>

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



namespace block
{




/*
 nBit について
 前2bytesが指数部 ( 00000000 00000000 ) 
 後ろ2bytseがマントリップ ( 00000000 00000000 ) // 0の個数
 */





struct BlockHeader
{
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


	size_t headerHash( std::shared_ptr<unsigned char> *ret );
	void print();

} __attribute__((packed)); 








struct Block
{
private:
//public:
	BlockHeader _header;
	std::shared_ptr<tx::Coinbase> _coinbase;
	std::vector< std::shared_ptr<tx::P2PKH> > _txVector;


public:
	Block(); //  

	//BlockHeader header(){ return _header; };
	BlockHeader* header(){ return &_header; };
	void header( std::shared_ptr<BlockHeader> target );
	void header( BlockHeader target );

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
	//size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ); // ヘッダとトランザクション部まとめて書き出すようなことはしない

	uint32_t time();

};


};


#endif // F4666B08_7795_43DE_89AA_6D147306F4CA



