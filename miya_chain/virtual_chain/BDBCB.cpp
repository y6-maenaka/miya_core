#include "BDBCB.h"

#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"


namespace miya_chain
{




BDBCB::BDBCB( std::shared_ptr<unsigned char> fromBlockHash )
{
  _blockCache._blockHash = fromBlockHash; 

  _status = static_cast<int>(BDState::BlockHashReceived);
}

BDBCB::BDBCB( std::shared_ptr<block::BlockHeader> fromBlockHeader ) 
{
  _body = fromBlockHeader; // ヘッダーをセット

  // ブロックハッシュキャッシュをセット
  std::shared_ptr<unsigned char> fromBlockHash;
  fromBlockHash = fromBlockHeader->headerHash();
  _blockCache._blockHash = fromBlockHash;

  _status = static_cast<int>(BDState::BlockHeaderReceived); // ステータスの変更
}

BDBCB::BDBCB( std::shared_ptr<block::Block> fromBlock )
{
  _body = fromBlock;

  // ブロックハッシュキャッシュをセット
  std::shared_ptr<unsigned char> fromBlockHash;
  fromBlockHash = fromBlock->blockHash();
  _blockCache._blockHash = fromBlockHash;
  _blockCache._height = fromBlock->height();

  _status = static_cast<int>(BDState::BlockBodyReceived);
}

int BDBCB::status()
{
  return _status;
}

void BDBCB::status( int target )
{
  _status = target;
}

std::shared_ptr<unsigned char> BDBCB::blockHash() const
{
	return _blockCache._blockHash;
}

unsigned int BDBCB::height()
{
  return _blockCache._height;
}

std::shared_ptr<unsigned char> BDBCB::prevBlockHash() const
{
  return _blockCache._blockHash;
  // return _body->header()->prevBlockHash();
}










void BDBCB::print()
{
	std::cout << "blockHash :: ";
	std::shared_ptr<unsigned char> blockHash = _blockCache._blockHash;
	for( int i=0; i<32; i++){
		printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";

	std::cout << "height :: " << static_cast<size_t>(this->height()) << "\n";

	std::cout << "status :: " << this->status() << "\n";

}












};
