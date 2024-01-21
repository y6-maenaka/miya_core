#include "prev_block_hash_as_key.h"

#include "./virtual_chain.h"
#include "./bd_filter.h"
#include "../block/block_header.h"


namespace miya_chain
{








PrevBlockHashAsKey::PrevBlockHashAsKey( std::shared_ptr<unsigned char> fromHash )
{
  if( fromHash == nullptr ) return;
  memcpy( _prevBlockHash , fromHash.get() , sizeof(_prevBlockHash) );
}

PrevBlockHashAsKey::PrevBlockHashAsKey( std::shared_ptr<block::BlockHeader> fromHeader )
{
  if( fromHeader == nullptr ) return;
  std::shared_ptr<unsigned char> prevBlockHash = fromHeader->prevBlockHash();

  if( prevBlockHash == nullptr ) return;
  memcpy( _prevBlockHash , prevBlockHash.get(), sizeof(_prevBlockHash) );
}


bool PrevBlockHashAsKey::operator==(const PrevBlockHashAsKey& key) const {
	return (memcmp( _prevBlockHash, key._prevBlockHash , sizeof(_prevBlockHash) ) == 0 );
}


bool PrevBlockHashAsKey::operator!=(const PrevBlockHashAsKey& key) const {
    return !(this->operator==(key));
}


std::shared_ptr<unsigned char> PrevBlockHashAsKey::prevBlockHash() const
{
  return std::make_shared<unsigned char>(*_prevBlockHash);
}


std::size_t PrevBlockHashAsKey::Hash::operator()(const PrevBlockHashAsKey& key) const
{
	std::string bytes(reinterpret_cast<const char*>(key._prevBlockHash), sizeof(key._prevBlockHash));
	return std::hash<std::string>()(bytes);
	//ダブルハッシュになって非効率的だが,unsigned char[32]をsize_tに変換する術を知らない
}

void PrevBlockHashAsKey::__printHash() const
{
  for( int i=0; i< sizeof(_prevBlockHash); i++ )
  {
	printf("%02X", _prevBlockHash[i] );
  } std::cout << "\n";
}






}
