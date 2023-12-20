#include "block_chain_iterator.h"

#include "../block/block.h"
#include "../block/block_header.h"
#include "../transaction/coinbase/coinbase.h"

#include "../miya_coin/local_strage_manager.h"



namespace miya_chain
{




bool BlockChainIterator::setupFromBlockHash( std::shared_ptr<unsigned char> from )
{
	std::shared_ptr<block::Block> thisBlock;
	thisBlock = _localStrageManager->readBlock( from );

  return true;
}


BlockChainIterator::BlockChainIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<unsigned char> blockHash )
{
	if( localStrageManager == nullptr || blockHash == nullptr ) return;

	_localStrageManager = localStrageManager;
	setupFromBlockHash( blockHash ); // 失敗する可能性もある

  if( _body ){ // キャッシュのセット
    _cache._blockHash = _body->blockHash();
    _cache._heigth = _body->height();
  }
  return;
}

BlockChainIterator::BlockChainIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<block::Block> body )
{
	if( localStrageManager == nullptr ) return;

	_body = body;
  if( _body ){
    _cache._blockHash = _body->blockHash();
    _cache._heigth = _body->height();
  }
  return;
}


std::shared_ptr<block::Block> BlockChainIterator::operator*()
{
	return _body;
}

BlockChainIterator& BlockChainIterator::operator--()
{
	std::shared_ptr<unsigned char> prevBlockHash = _body->header()->prevBlockHash();
	// ブロックハッシュから保体をセットアップ
	this->setupFromBlockHash( prevBlockHash );
	return *this;
}

std::shared_ptr<unsigned char> BlockChainIterator::blockHash() const
{
	return _cache._blockHash;
}

long long BlockChainIterator::height() const
{
	return _cache._heigth;
}


bool BlockChainIterator::operator==( const BlockChainIterator &si ) const
{
	if( memcmp( _body->blockHash().get() , si.blockHash().get() , 32 ) == 0 ) return true;
	return false;
}


bool BlockChainIterator::operator!=( const BlockChainIterator &si ) const
{
	return !(this->operator==(si));
}








}
