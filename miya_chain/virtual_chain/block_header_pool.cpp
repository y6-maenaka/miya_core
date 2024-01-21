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

  PrevBlockHashAsKey pbKey( prevBlockHash );
  short duplicateCount = 0;
  duplicateCount = _pool_prevBlockHash.count( pbKey ); // 要素を追加する前のカウントを取得しておく

  auto insertRet = _pool_prevBlockHash.insert({ pbKey , target }); 
  if( !(insertRet->second) ) return std::make_pair( false , duplicateCount ); // 挿入失敗 

  BlockHashAsKey bKey( target );
  _pool_blockHsah.insert( {bKey, target} );

  return std::make_pair( true , ++duplicateCount );
}


std::shared_ptr<block::BlockHeader> VirtualChain::BlockHeaderPool::findByBlockHash( std::shared_ptr<unsigned char> blockHash )
{
  BlockHashAsKey key( blockHash );

  auto ret = _pool_blockHsah.find( key );
  if( ret == _pool_blockHsah.end() ) return nullptr;

  return ret->second;
}

std::vector< std::shared_ptr<block::BlockHeader> > VirtualChain::BlockHeaderPool::findByPrevBlockHash( std::shared_ptr<unsigned char> prevBlockHash )
{
  std::vector< std::shared_ptr<block::BlockHeader> >  retVector;
  PrevBlockHashAsKey key( prevBlockHash );

  auto ret = _pool_prevBlockHash.equal_range( key );
  for( auto itr = ret.first; itr != ret.second; itr++ ){
	retVector.push_back( itr->second );
  }

  return retVector;
}


void VirtualChain::BlockHeaderPool::__printPoolSortWithPrevBlockHash()
{
  std::cout << "===== <Virtual Chain Lists> =====" << "\n";

  std::cout << "カウント : " << _pool_prevBlockHash.size() << "\n";

  int i=0;
  for( auto itr : _pool_prevBlockHash )
  {
	std::cout << "( << " << i << "<< ) ";
	itr.first.__printHash();
	i++;
	std::cout << "--------------------------" << "\n";
  }
}








};

