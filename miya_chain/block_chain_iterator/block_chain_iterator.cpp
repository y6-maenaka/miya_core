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
	if( thisBlock == nullptr ) return false;

	_body = thisBlock; // 格納
	/*
	std::shared_ptr<unsigned char> blockHash; size_t blockHashLength;
	blockHashLength = thisBlock->blockHash( &blockHash );
	for( int i=0; i<blockHashLength; i++ ){
			printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";
	printf("ThisBlock Pointer :: %p\n", thisBlock.get() );
	*/
  return true;
}

BlockChainIterator::BlockChainIterator( const BlockChainIterator& bcitr )
{
	this->_body = bcitr._body;
	this->_localStrageManager = bcitr._localStrageManager;
	this->_cache = bcitr._cache;
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

	/*
	std::cout << "--------------" << "\n";
	std::cout << "block Hash :: ";
	for( int i=0; i<32; i++ ){
			printf("%02X", _cache._blockHash.get()[i]);
	} std::cout << "\n";
	std::cout << "--------------" << "\n";
	*/

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

std::shared_ptr<BlockChainIterator> BlockChainIterator::prev()
{
	std::shared_ptr<unsigned char> prevBlockHash = _body->header()->prevBlockHash();
	if( prevBlockHash == nullptr ) return nullptr;

	std::shared_ptr<BlockChainIterator> ret = std::shared_ptr<BlockChainIterator>( new BlockChainIterator( _localStrageManager , prevBlockHash ) );
	return ret;
}

std::shared_ptr<block::Block> BlockChainIterator::block()
{
  return _body;
}

std::shared_ptr<block::BlockHeader> BlockChainIterator::header()
{
  if( _body == nullptr ) return nullptr;
  return _body->header();
}

std::shared_ptr<unsigned char> BlockChainIterator::blockHash() const
{
	return _cache._blockHash;
}

long long BlockChainIterator::height() const
{
	return _cache._heigth;
}

bool BlockChainIterator::isHead()
{
	unsigned char addrZero[32]; memset( addrZero, 0x00 , sizeof(addrZero) );
	return ( memcmp( _cache._blockHash.get() , addrZero, sizeof(addrZero) ) == 0 );
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

void BlockChainIterator::__printHeader()
{
	if( _body == nullptr ){
		std::cout << "(@) Block is Empty" << "\n";
		return;
	}
	return _body->header()->print();
}








}
