#include "block_c_iterator.h"

#include "./block.h"
#include "./block_header.h"
#include "../transaction/coinbase/coinbase.h"

#include "../miya_coin/local_strage_manager.h"

namespace block
{


BlockCIterator::BlockCIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<unsigned char> bodyBlockHash )
{
	if( localStrageManager == nullptr || bodyBlockHash == nullptr ) return;
	
	_localStrageManager = localStrageManager;
	setupFromBlockHash( bodyBlockHash );
}



BlockCIterator::BlockCIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<Block> body )
{
	if( localStrageManager == nullptr ) return;

	if( body == nullptr ){
		_body = nullptr;
		_blockHashCache = nullptr;
		_heigthCache = -1;
	}

	_body = body;
	_blockHashCache = body->blockHash();
	_heigthCache = body->coinbase()->height();
}



void BlockCIterator::setupFromBlockHash( std::shared_ptr<unsigned char> from )
{
	std::shared_ptr<Block> thisBlock;
	thisBlock = _localStrageManager->readBlock( from );
	if( thisBlock == nullptr ){
		_body = nullptr;
		_blockHashCache = nullptr;
		_heigthCache = -1;

	}

	_body = thisBlock;
	_blockHashCache = from;
	_heigthCache = _body->coinbase()->height();
}




std::shared_ptr<Block> BlockCIterator::operator*()
{
	return _body;
}


BlockCIterator& BlockCIterator::operator--()
{
	std::shared_ptr<unsigned char> prevBlockHash = _body->header()->prevBlockHash();

	// ブロックハッシュから保体をセットアップ
	this->setupFromBlockHash( prevBlockHash );

	return *this;
}


std::shared_ptr<unsigned char> BlockCIterator::blockHash() const
{
	return _blockHashCache;
}

long long BlockCIterator::heigth() const
{
	return _heigthCache;
}



bool BlockCIterator::operator==( const BlockCIterator &si ) const
{
	if( memcmp( _body->blockHash().get() , si.blockHash().get() , 32 ) == 0 ) return true;
	return false;
}



bool BlockCIterator::operator!=( const BlockCIterator &si ) const
{ 
	return !(this->operator==(si));
}








}
