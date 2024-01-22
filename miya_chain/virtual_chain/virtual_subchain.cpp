#include "virtual_subchain.h"

#include "../../shared_components/hash/sha_hash.h"

#include "../block/block.h"
#include "../block/block_header.h"

#include "../block_chain_iterator/block_chain_iterator.h"

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





VirtualSubChain::VirtualSubChain( std::shared_ptr<block::BlockHeader> startBlockHeader,
								  std::shared_ptr<unsigned char> stopHash,
								  BHPoolFinder bhPoolFinder , 
								  PBHPoolFinder pbhPoolFinder )
  : _context(startBlockHeader) , _bhPoolFinder(bhPoolFinder), _pbhPoolFinder(pbhPoolFinder) , _stopHash(stopHash)
{ 
  _context._latestBlockHeader = startBlockHeader; // 初期化時の最後尾ブロックヘッダーはstartBlockHeaderにしておく
  return;
}


size_t VirtualSubChain::chainDigest( std::shared_ptr<unsigned char> *ret ) const
{
  std::shared_ptr<unsigned char> joinedHash = std::shared_ptr<unsigned char>( new unsigned char[32*2] );
  std::shared_ptr<unsigned char> startBlockHash = _context._startBlockHeader->headerHash(); // スタートブロックのハッシュ
  std::shared_ptr<unsigned char> latestBlockHash;

  if( _context._latestBlockHeader == nullptr )
  {
	  latestBlockHash = std::shared_ptr<unsigned char>( new unsigned char[32] );
	  memset( latestBlockHash.get(), 0x00 , 32 );
  }
  else
  {
	  latestBlockHash = _context._latestBlockHeader->headerHash(); // 最後尾ブロックのハッシュ
  }
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


std::shared_ptr<block::BlockHeader> VirtualSubChain::latestBlockHeader()
{
  return _context._latestBlockHeader;
}


void VirtualSubChain::update()
{
  _updatedAt = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

uint32_t VirtualSubChain::updatedAt() const
{
  return _updatedAt;
}

void VirtualSubChain::build( std::shared_ptr<block::BlockHeader> latestBlockHeader )
{
  _context._latestBlockHeader = latestBlockHeader;
}


void VirtualSubChain::extend( int collisionAction )
{
  std::cout << "VirtualSubChain::extend() called" << "\n";

  // std::unique_lock<std::shared_mutex> lock(_mtx);
  if( _stopHash != nullptr && memcmp( _context.latestBlockHash().get(), _stopHash.get() , 32 ) == 0 ) return; // ブロックの最後尾がstopHashに達していたら即座に戻る

  auto popRet = _pbhPoolFinder( _context.latestBlockHash() ); // 検索結果が複数個だった場合はどれを選択する
  if( popRet.empty() )
  {  // pool内に対象のheaderが存在しない場合は戻る
	std::cout << "(@) 延長ヘッダーは存在しません" << "\n";
	return;
  }
  
  if( popRet.size() > 1 )
  {
	switch( collisionAction ) // 最後尾ブロックを参照するブロックヘッダーが複数存在する場合
	{
	  case ( 0 ):  // 停止 : 延長を停止して戻す
		{
		  std::cout << "(@) 延長ヘッダー重複 : 延長停止" << "\n";
		  return;
		}
	  case ( 1 ): // ランダム : どれかをランダムに選択して延長を続ける
		{
		  std::cout << "(@) 延長ヘッダー重複 : ランダム延長" << "\n";
		  std::srand(static_cast<unsigned int>(std::time(nullptr))); // 乱数シードの設定
		  _context._latestBlockHeader = popRet.at( std::rand() % popRet.size() );
		  this->update(); // 最後尾を更新した場合はupdateする
		  break;
		}
	};
  }
  else // 検索結果が1つの場合
  {
	std::cout << "(@) 仮想チェンサブチェーンの最先端ヘッダーを更新します" << "\n";
	// std::cout << "PrevBlockHashFinder の検索結果は1つです" << "\n";
	// std::shared_ptr<block::BlockHeader>	 __latestBlockHeader = popRet.at(0);
	_context._latestBlockHeader = popRet.at(0); // 最後尾ブロックを更新
	this->update(); // 最後尾を更新した場合はupdateする
  }
  
  return extend( collisionAction );
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

std::size_t VirtualSubChain::Hash::operator()( const VirtualSubChain &sc ) const  // 動作が不安定な関数 string格納辺り
{
  std::shared_ptr<unsigned char> chainDigest; size_t chainDigestLength;
  chainDigestLength = sc.chainDigest( &chainDigest );

  /*
  std::cout << "ChainDigetLength :: " << chainDigestLength << "\n";
  std::cout <<  "STATE_HASH_LENGTH :: " << STATE_HASH_LENGTH << "\n";

  for(int i=0; i<STATE_HASH_LENGTH; i++ ){
	printf("%02X", chainDigest.get()[i] );
  } std::cout << "\n";
  std::cout << "DONE" << "\n";
  */


  // std::string bytes( reinterpret_cast<const char*>( sc.chainDigest().get(), STATE_HASH_LENGTH ));
  
  std::string bytes( reinterpret_cast<char*>(chainDigest.get()), 20 );
  return std::hash<std::string>()(bytes);

 
  
  // std::string bytes(20,'\0');
  // std::string bytes( reinterpret_cast<const char*>(chainDigest.get()), 20 );
  // std::cout << "__bytes :: " << bytes.size() << "\n";
  // bytes.assign( reinterpret_cast<char*>(chainDigest.get(), 20 ) );
  
  /*
  std::cout << "   before" << "\n";
  std::cout << "bytes length :: " << bytes.size() << "\n";
  for( int i=0; i<bytes.size(); i++ )
	printf("%02X", bytes[i] );
  std::cout << "\n";
  std::cout << "DONE 2" << "\n";
  */

  return std::hash<std::string>()(bytes);
}


std::vector< std::shared_ptr<block::BlockHeader> > VirtualSubChain::exportChainVector()
{
  std::vector< std::shared_ptr<block::BlockHeader> > ret;

  std::shared_ptr<unsigned char> blockHash; 
  std::shared_ptr<block::BlockHeader> blockHeader;

  blockHeader = _context._latestBlockHeader;

  while( blockHeader != nullptr ) 
  {
	ret.push_back( blockHeader );
	blockHash = ret.back()->prevBlockHash();

	blockHeader = _bhPoolFinder( blockHash );
  }
  
  return ret;
}




void VirtualSubChain::__printChainDigest() const
{
  std::shared_ptr<unsigned char> chainDigest; size_t chainDigestLength;
  chainDigestLength = this->chainDigest( &chainDigest );
  std::cout << "\x1b[36m";
  std::cout << "ChainDigest :: " ;
  for( int i=0; i<chainDigestLength; i++ )
	printf("%02X", chainDigest.get()[i] );
  std::cout << "\n";
  std::cout << "\x1b[39m";
}


};
