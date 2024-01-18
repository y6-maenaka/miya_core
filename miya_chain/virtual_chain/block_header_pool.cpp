#include "virtual_chain.h"

#include "../block/block.h"
#include "../block/block_header.h"
#include "../transaction/coinbase/coinbase.h"



namespace miya_chain
{









// VirtualChain || BlockHeaderPool


std::pair< bool, short > VirtualChain::BlockHeaderPool::push( std::shared_ptr<block::BlockHeader> target )
{
  // targetBlockHeaderからprevBlockHashの取り出し
  std::shared_ptr<unsigned char> prevBlockHash = target->prevBlockHash(); 
  if( prevBlockHash == nullptr ) return std::make_pair( false , 0 ); // 対象が不正の場合はnullptrを返す

  PrevBlockHashAsKey key( prevBlockHash );
  short duplicateCount = 0;
  duplicateCount = _pool.count( key ); // 要素を追加する前のカウントを取得しておく

  auto insertRet = _pool.insert({ key , target }); 
  if( !(insertRet->second) ) return std::make_pair( false , duplicateCount ); // 挿入失敗 

  return std::make_pair( true , ++duplicateCount );
}


std::vector< std::shared_ptr<block::BlockHeader> > VirtualChain::BlockHeaderPool::find( std::shared_ptr<unsigned char> prevBlockHash )
{
  std::vector< std::shared_ptr<block::BlockHeader> >  retVector;
  PrevBlockHashAsKey key( prevBlockHash );

  auto ret = _pool.equal_range( key );
  for( auto itr = ret.first; itr != ret.second; itr++ ){
	retVector.push_back( itr->second );
  }

  return retVector;
}








};

