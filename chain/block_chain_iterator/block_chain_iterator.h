#ifndef D9C3DA28_8439_4D06_96A0_3D2D2B5EF526
#define D9C3DA28_8439_4D06_96A0_3D2D2B5EF526


#include <memory>

#include <chain/block/block.h>
#include <chain/block/block_header.h>
#include <chain/transaction/coinbase/coinbase.hpp>

#include <chain/miya_coin/local_strage_manager.h>


namespace chain
{
  class BlockLocalStrageManager;
};



namespace block
{
  struct Block;
	struct BlockHeader;
}



namespace chain
{


class BlockChainIterator // const iterator
{
private:
	std::shared_ptr<struct Block> _body = nullptr;
	// std::shared_ptr<unsigned char> _blockHashCache;
	std::shared_ptr<BlockLocalStrageManager> _localStrageManager = nullptr;
	bool setupFromBlockHash( std::shared_ptr<unsigned char> from ); // 実質的なコンストラクタ

  struct // 毎回bodyから取得するのは面倒だからキャッシュを設ける
  {
    std::shared_ptr<unsigned char> _blockHash = nullptr;
	   long long _heigth = -1;
  } _cache;

public:
  BlockChainIterator( const BlockChainIterator& bcitr );
  BlockChainIterator( std::shared_ptr<BlockLocalStrageManager> localStrageManager , std::shared_ptr<unsigned char> blockHash ); // ブロックボディの取得はコンストラクタ内で行う
  BlockChainIterator( std::shared_ptr<BlockLocalStrageManager> localStrageManager , std::shared_ptr<Block> body ); // ブロック本体からイテレータを作成する

  std::shared_ptr<struct Block> operator*(); // デリファレンス
  BlockChainIterator &operator--(); // ブロックチェーンの逆方向参照
  std::shared_ptr<BlockChainIterator> prev();

  std::shared_ptr<struct Block> block();
  std::shared_ptr<struct BlockHeader> header();
  std::shared_ptr<unsigned char> blockHash() const;
  long long height() const;

  bool isHead();

  bool operator==( const BlockChainIterator &si ) const;
  bool operator!=( const BlockChainIterator &si ) const;

  void __printHeader();
};


};


#endif // D9C3DA28_8439_4D06_96A0_3D2D2B5EF526
