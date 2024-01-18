#include "virtual_subchain.h"

#include "../../shared_components/hash/sha_hash.h"

#include "../block/block.h"
#include "../block/block_header.h"

#include "../message/command/getblocks.h"


namespace miya_chain
{




VirtualSubChain::Context::Context( std::shared_ptr<block::BlockHeader> startBlockHeader )
  : _startBlockHeader( startBlockHeader )
{
  return;
}






std::shared_ptr<unsigned char> VirtualSubChain::Context::latestBlockHash()
{
  return _latestBlockHeader->headerHash();
}





VirtualSubChain::VirtualSubChain( std::shared_ptr<block::BlockHeader> startBlockHeader )
  : _context(startBlockHeader)
{ 
}

size_t VirtualSubChain::chainDigest( std::shared_ptr<unsigned char> *ret ) const
{
  std::shared_ptr<unsigned char> joinedHash = std::shared_ptr<unsigned char>( new unsigned char[32*2] );
  std::shared_ptr<unsigned char> startBlockHash = _context._startBlockHeader->headerHash(); // スタートブロックのハッシュ
  std::shared_ptr<unsigned char> latestBlockHash = _context._latestBlockHeader->headerHash(); // 最後尾ブロックのハッシュ
  memcpy( joinedHash.get() , startBlockHash.get() , 32 );
  memcpy( joinedHash.get() + 32 , latestBlockHash.get(), 32 );
 
  return hash::SHAHash( joinedHash , 32*2 , ret , "sha1" );
}

std::shared_ptr<unsigned char> VirtualSubChain::chainDigest() const
{
  std::shared_ptr<unsigned char> ret;
  this->chainDigest( &ret );
  return ret;
}


void VirtualSubChain::update()
{
  _updatedAt = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

uint32_t VirtualSubChain::updatedAt() const
{
  return _updatedAt;
}


void VirtualSubChain::extend( std::function<std::vector<std::shared_ptr<block::BlockHeader>>(std::shared_ptr<unsigned char>)> popCallback , std::shared_ptr<unsigned char> stopHash, int collisionAction )
{
  // std::unique_lock<std::shared_mutex> lock(_mtx);
  if( stopHash != nullptr && memcmp( _context.latestBlockHash().get(), stopHash.get() , 32 ) == 0 ) return; // ブロックの最後尾がstopHashに達していたら即座に戻る

  auto popRet = popCallback( _context.latestBlockHash() ); // 検索結果が複数個だった場合はどれを選択する
  if( popRet.empty() ) return; // pool内に対象のheaderが存在しない場合は戻る
  if( popRet.size() > 1 )
  {
	switch( collisionAction ) // 最後尾ブロックを参照するブロックヘッダーが複数存在する場合
	{
	  case ( 0 ):  // 停止 : 延長を停止して戻す
		{
		  return;
		}
	  case ( 1 ): // ランダム : どれかをランダムに選択して延長を続ける
		{
		  std::srand(static_cast<unsigned int>(std::time(nullptr))); // 乱数シードの設定
		  _context._latestBlockHeader = popRet.at( std::rand() % popRet.size() );
		  this->update(); // 最後尾を更新した場合はupdateする
		  break;
		}
	};
  }
  else
  {
	_context._latestBlockHeader = popRet.at(0); // 最後尾ブロックを更新
	this->update(); // 最後尾を更新した場合はupdateする
  }
  
  return extend( popCallback , stopHash , collisionAction );
}


MiyaChainCommand VirtualSubChain::extendCommand()
{
  MiyaChainMSG_GETBLOCKS command;
  MiyaChainCommand wrapedCommand;

  std::shared_ptr<unsigned char> latestBlockHash = this->_context.latestBlockHash();
  command.startHash( latestBlockHash );

  wrapedCommand = command;
  return wrapedCommand;
}




bool VirtualSubChain::operator==( const VirtualSubChain &sc ) const
{
  std::shared_ptr<unsigned char> chainDigest; size_t chainDigestLength;
  chainDigestLength = sc.chainDigest( &chainDigest );

  return ( memcmp( this->chainDigest().get() , chainDigest.get() , chainDigestLength ) == 0 );
}

bool VirtualSubChain::operator!=( const VirtualSubChain &sc ) const
{
  return !(this->operator==(sc));
}

std::size_t VirtualSubChain::Hash::operator()( const VirtualSubChain &sc ) const 
{
  std::string bytes( reinterpret_cast<const char*>( sc.chainDigest().get(), STATE_HASH_LENGTH ));
  return std::hash<std::string>()(bytes);
}



};
