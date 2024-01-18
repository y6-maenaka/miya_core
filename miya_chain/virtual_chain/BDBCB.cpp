#include "BDBCB.h"

#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"


namespace miya_chain
{




BDBCB::BDBCB( std::shared_ptr<unsigned char> fromBlockHash ) : _block{nullptr}
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
	return _block->header()->prevBlockHash();
}










void BDBCB::print()
{
	std::cout << "blockHash :: ";
	std::shared_ptr<unsigned char> blockHash;
	_block->blockHash( &blockHash );
	for( int i=0; i<32; i++){
		printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";

	std::cout << "height :: " << static_cast<size_t>(this->height()) << "\n";

	std::cout << "status :: " << this->status() << "\n";

}












};
