#ifndef D9C3DA28_8439_4D06_96A0_3D2D2B5EF526
#define D9C3DA28_8439_4D06_96A0_3D2D2B5EF526


#include <memory>



namespace miya_chain
{
	class BlockLocalStrageManager;
};



namespace block
{
  struct Block;
	struct BlockHeader;
}



namespace miya_chain
{









class BlockChainIterator // const iterator
{
private:
	std::shared_ptr<block::Block> _body = nullptr;
	// std::shared_ptr<unsigned char> _blockHashCache;
	std::shared_ptr<miya_chain::BlockLocalStrageManager> _localStrageManager = nullptr;
	bool setupFromBlockHash( std::shared_ptr<unsigned char> from ); // 実質的なコンストラクタ

  struct // 毎回bodyから取得するのは面倒だからキャッシュを設ける
  {
    std::shared_ptr<unsigned char> _blockHash = nullptr;
	   long long _heigth = -1;
  } _cache;

public:
  BlockChainIterator( const BlockChainIterator& bcitr );
  BlockChainIterator( std::shared_ptr<BlockLocalStrageManager> localStrageManager , std::shared_ptr<unsigned char> blockHash ); // ブロックボディの取得はコンストラクタ内で行う
  BlockChainIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<block::Block> body ); // ブロック本体からイテレータを作成する

  std::shared_ptr<block::Block> operator*(); // デリファレンス
  BlockChainIterator &operator--(); // ブロックチェーンの逆方向参照
  std::shared_ptr<BlockChainIterator> prev();

  std::shared_ptr<block::Block> block();
  std::shared_ptr<block::BlockHeader> header();
  std::shared_ptr<unsigned char> blockHash() const;
  long long height() const;

  bool isHead();

  bool operator==( const BlockChainIterator &si ) const;
  bool operator!=( const BlockChainIterator &si ) const;

  void __printHeader();
};











};




#endif // D9C3DA28_8439_4D06_96A0_3D2D2B5EF526
