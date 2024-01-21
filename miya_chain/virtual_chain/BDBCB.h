#ifndef C2891934_CA8E_459D_A649_913804F6BED3
#define C2891934_CA8E_459D_A649_913804F6BED3

#include <mutex>
#include <memory>
#include <variant>

namespace block
{
  struct Block;
  struct BlockHeader;
};





namespace miya_chain
{


enum class BDState : int
{
	BlockHashReceived = 1,
	// BlockHeaderNotfound,
	// BlockNotfound,
	BlockHeaderReceived = 2, // ブロックヘッダの取得完了
	BlockHeaderValidated = 3, // ブロックヘッダの検証完了
	
	BlockBodyReceived = 4 , // ブロック本体の取得完了
	BlockBodyValidated = 5,  // ブロックの本体検証完了
	
	BlockStored = 6, // ブロック本体の取得がが完了し,ローカルに保存済み

	Error = -2, // なんらかのエラー発生
	Empty = 0,
};





struct BDBCB // ( Block Donwload Block Control Block)
{
private:
	std::variant< std::shared_ptr<block::Block> , std::shared_ptr<block::BlockHeader> > _body;
	int _status;

  struct
  {
      std::shared_ptr<unsigned char> _blockHash;
      uint32_t _height;
  } _blockCache;

public:
  BDBCB( std::shared_ptr<unsigned char> fromBlockHash ); // ブロックハッシュから作成(StateはBlockHashReceived)
  BDBCB( std::shared_ptr<block::BlockHeader> fromBlockHeader );
  BDBCB( std::shared_ptr<block::Block> fromBlock );

  int status();
  void status( int target );
  // 間接Getterメソッド
  std::shared_ptr<unsigned char> blockHash() const; // キャッシュから呼び出す
  unsigned int height();
  std::shared_ptr<unsigned char> prevBlockHash() const;

  // debug
  void print();
};







};



#endif // C2891934_CA8E_459D_A649_913804F6BED3
