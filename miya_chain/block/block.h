#ifndef F4666B08_7795_43DE_89AA_6D147306F4CA
#define F4666B08_7795_43DE_89AA_6D147306F4CA



#include <vector>
#include <memory>


namespace tx
{
	class P2PKH;
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
	BlockHeader _header;

	//std::shared_ptr<tx::CoinBase> _coinbase;
	//std::vector std::shared_ptr<tx::P2PKH>> _txVector; // トランザクションのリスト
	std::vector<tx::P2PKH> _txVector; // トランザクションのリスト
};


};


#endif // F4666B08_7795_43DE_89AA_6D147306F4CA



