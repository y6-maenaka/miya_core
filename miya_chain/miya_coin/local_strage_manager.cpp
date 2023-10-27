#include "local_strage_manager.h" 

#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"
#include "../transaction/p2pkh/p2pkh.h"





namespace miya_chain
{




BlockContainer::BlockContainer()
{
	_block = std::make_shared<block::Block>();
}




BlockContainer::BlockContainer( std::shared_ptr<block::Block> block )
{
	_block = block;
}



size_t BlockContainer::size()
{
	return static_cast<size_t>(_meta._size);
}


size_t BlockContainer::txCount()
{
	return static_cast<size_t>(_meta._txCount);
}




size_t BlockContainer::exportRawFormated( std::shared_ptr<unsigned char> *retRaw )
{
	if( _block == nullptr ){
		*retRaw = nullptr;
		return 0;
	}

	// ブロックヘッダの書き出し
	std::shared_ptr<unsigned char> rawBlockHeader; size_t rawBlockHeaderLength;
	rawBlockHeaderLength = _block->exportHeader( &rawBlockHeader );

	// coinbaseの書き出し
	std::shared_ptr<unsigned char> rawCoinbase; size_t rawCoinbaseLength;
	rawCoinbaseLength = _block->coinbase()->exportRaw( &rawCoinbase );

	
	// トランザクションの書き出し
	std::vector< std::shared_ptr<tx::P2PKH> > txVector = _block->txVector();
	std::vector< std::pair<std::shared_ptr<unsigned char> , size_t> > rawTxVector;
	size_t rawTxsLength = 0; // 生トランザクション"全体"サイズ
	for( auto itr : txVector )
	{
		std::shared_ptr<unsigned char> rawTx; size_t rawTxLength;
		rawTxLength = itr->exportRaw( &rawTx );
		rawTxVector.push_back( std::make_pair( rawTx , rawTxLength ) );
		rawTxsLength += rawTxLength;
	}
	
	_meta._txCount = static_cast<uint32_t>(rawTxVector.size());

	
	size_t retLength = sizeof(_meta) + rawBlockHeaderLength + rawTxsLength;
	_meta._size = static_cast<uint32_t>(retLength); 

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
	
	size_t formatPtr = 0;
	memcpy( (*retRaw).get() + formatPtr , &_meta , sizeof(_meta) );  formatPtr += sizeof(_meta); // metaの書き出し
	memcpy( (*retRaw).get() + formatPtr , rawBlockHeader.get() , rawBlockHeaderLength ); formatPtr += rawBlockHeaderLength; // ブロックヘッダの書き出し
	for( auto itr : rawTxVector ) {  // トランザクションの書き出し
		memcpy( (*retRaw).get() + formatPtr, (itr.first).get() , itr.second );  
		formatPtr += itr.second;
	}


	
}





void LocalFileController::Meta::actualDataSize( size_t target )
{
	_actualDataSize = static_cast<uint32_t>( target );
	msync( this , BLK_REV_META_BLOCK_SIZE , MS_SYNC );
	// 同期する
}

size_t LocalFileController::Meta::actualDataSize()
{
	return static_cast<size_t>( _actualDataSize );
}




LocalFileController::LocalFileController( std::string filePath )
{
	_systemPageSize = sysconf( _SC_PAGESIZE ); // ランタイムで取得する
	_fd = open( filePath.c_str() , O_RDWR | O_CREAT, (mode_t)(0600) );

	if( _fd < 0 ) return;
	if( stat( filePath.c_str() , &_st ) != 0 ) return; 
	
	std::cout << "Open " << filePath << " with FD :: " << _fd << "\n";

	_mapping._addr = mmap( NULL , BLK_REV_META_BLOCK_SIZE , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , 0 );
	_meta = static_cast<LocalFileController::Meta*>(_mapping._addr);
	_meta->actualDataSize( BLK_REV_META_BLOCK_SIZE );
}



std::shared_ptr<BlockContainer> LocalFileController::read( off_t offset ) // サイズを指定してもらう
{

	std::shared_ptr<BlockContainer> ret;
	_mapping._addr = mmap( NULL , sizeof( struct BlockContainer::Meta ) , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , 0 ); 
	_mapping._size = sizeof( struct BlockContainer::Meta );

	memcpy( &(ret->_meta) , _mapping._addr , sizeof(struct BlockContainer::Meta) );
	munmap( _mapping._addr , _mapping._size );

	std::shared_ptr<unsigned char> rawBlock; size_t rawBlockLength;
	rawBlockLength = ret->size();
	
	_mapping._addr = mmap( NULL , rawBlockLength , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , 0 ); // ブロック全体をマッピングする
	_mapping._size = rawBlockLength;


	// ブロックヘッダの取り込み
	size_t currentPtr = 0;
	currentPtr += ret->_block->header()->importRawSequentially( _mapping._addr );
	
	// coinbaseの取り込み
	currentPtr += ret->_block->coinbase()->importRawSequentially( static_cast<unsigned char*>(_mapping._addr) + currentPtr );

	// トランザクションの取り込み
	for( int i=0; i<ret->txCount(); i++ ){
		std::shared_ptr<tx::P2PKH> readedTx = std::make_shared<tx::P2PKH>();
		currentPtr += readedTx->importRawSequentially( static_cast<unsigned char*>(_mapping._addr) + currentPtr );
		ret->_block->add( readedTx );
	}


	msync( _mapping._addr , _mapping._size , MS_SYNC );
}



// どのファイル//Offsetを返却する
long LocalFileController::write( std::shared_ptr<BlockContainer> container )
{
	// データの最後尾はどこか
	size_t dataEnd = _meta->actualDataSize();

	std::shared_ptr<unsigned char> rawBlockContainer; size_t rawBlockContainerLength;
	rawBlockContainerLength = container->exportRawFormated( &rawBlockContainer );  // ブロックコンテナの書き出し


	if( dataEnd + rawBlockContainerLength >= DEFAULT_BLK_REV_MAX_BYTES ) return -1; // 次のファイルに書き込む

	ftruncate( _fd , rawBlockContainerLength ); // ファイルサイズの拡張
	off_t offset = ( dataEnd / _systemPageSize ) /* ページサイズ何個分見送るか*/;
	off_t mappingInOffset = dataEnd % _systemPageSize; 
	_mapping._addr = mmap( NULL , rawBlockContainerLength , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , (offset * _systemPageSize) );
	_mapping._size = rawBlockContainerLength;
	memcpy( static_cast<unsigned char *>(_mapping._addr) + mappingInOffset , rawBlockContainer.get() , rawBlockContainerLength );

	msync( _mapping._addr , rawBlockContainerLength , MS_SYNC );
	return dataEnd;
}



LocalFileController::~LocalFileController()
{
	munmap( _meta , BLK_REV_META_BLOCK_SIZE );
	munmap( _mapping._addr , _mapping._size );
	close( _fd );
}


















void BlockLocalStrageManager::writeBlock( std::shared_ptr<block::Block> targetBlock )
{
}


std::shared_ptr<block::Block> BlockLocalStrageManager::readBlock( std::shared_ptr<unsigned char> blockHash )
{
}




std::shared_ptr<tx::P2PKH> BlockLocalStrageManager::readTx( std::shared_ptr<unsigned char> txHash )
{

}




}
