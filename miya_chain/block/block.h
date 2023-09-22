#ifndef F4666B08_7795_43DE_89AA_6D147306F4CA
#define F4666B08_7795_43DE_89AA_6D147306F4CA



#include <vector>
#include <memory>
#include <iostream>
#include <functional>


namespace tx
{
	struct P2PKH;
	struct Coinbase;
}



namespace block
{







struct BlockHeader
{
	int32_t _version;
	unsigned char _previousBlockHeaderHash[32];
	unsigned char _merkleRootHash[32];
	uint32_t _time; // このブロックが生成された時のタイムスタンプ
	uint32_t _nBits; // 採掘難易度 目標のnonce値
	uint32_t _nonce; // ナンス値

} __attribute__((packed)); 








struct Block
{
private:
//public:
	BlockHeader _header;
	std::shared_ptr<tx::Coinbase> _coinbase;
	// std::vector<tx::P2PKH> _txVector; // トランザクションのリスト
	std::vector< std::shared_ptr<tx::P2PKH> > _txVector;


public:
	Block();
	BlockHeader header(){ return _header; };
	void header( std::shared_ptr<BlockHeader> target );
	void header( BlockHeader target );

	std::vector< std::shared_ptr<tx::P2PKH> > txVector();

	void coinbase( std::shared_ptr<tx::Coinbase> coinbase ); // setter
	void add( std::shared_ptr<tx::P2PKH> p2pkh ); // getter

	unsigned int  calcMerkleRoot( std::shared_ptr<unsigned char> *ret );
};


};


#endif // F4666B08_7795_43DE_89AA_6D147306F4CA



