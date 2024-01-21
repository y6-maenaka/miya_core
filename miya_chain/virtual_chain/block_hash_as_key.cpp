#include "block_hash_as_key.h"

#include "./virtual_chain.h"
#include "./bd_filter.h"
#include "../block/block_header.h"




namespace miya_chain
{



BlockHashAsKey::BlockHashAsKey( unsigned char* from )
{
  if( from == nullptr ) return;
  memcpy( _blockHash , from , sizeof(_blockHash) );
}

BlockHashAsKey::BlockHashAsKey( std::shared_ptr<unsigned char> from ) : BlockHashAsKey(from.get())
{
  return;
}

BlockHashAsKey::BlockHashAsKey( std::shared_ptr<block::BlockHeader> fromHeader )
{
  std::shared_ptr<unsigned char> blockHash = fromHeader->headerHash();
  memcpy( _blockHash, blockHash.get() , sizeof(_blockHash));
}


bool BlockHashAsKey::operator==(const BlockHashAsKey& key) const {
	return (memcmp( _blockHash, key._blockHash , sizeof(_blockHash) ) == 0 );
}


bool BlockHashAsKey::operator!=(const BlockHashAsKey& key) const {
    return !(this->operator==(key));
}


std::size_t BlockHashAsKey::Hash::operator()(const BlockHashAsKey& key) const
{
	std::string bytes(reinterpret_cast<const char*>(key._blockHash), sizeof(key._blockHash));
	return std::hash<std::string>()(bytes);
	//std::cout << "KeyHash :: " << ret << "\n";
	//ダブルハッシュになって非効率的だが,unsigned char[32]をsize_tに変換する術を知らない
}





void BlockHashAsKey::printHash() const
{
	std::cout << "Key(BlockHash) :: ";
	for( int i=0; i<sizeof(_blockHash); i++ ){
		printf("%02X", _blockHash[i] );
	} std::cout << "\n";
}






















}
