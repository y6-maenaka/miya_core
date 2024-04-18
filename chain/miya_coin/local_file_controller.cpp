#include "./local_strage_manager.h"


#include "../block/block.h"
#include "../transaction/coinbase/coinbase.hpp"
#include "../transaction/p2pkh/p2pkh.h"


namespace chain
{


LocalFileController::LocalFileController( std::string filePath )
{
	// meta先頭にフォーマット済みか否かのバイト列を配置するのもあり
	bool isNewFile = false; // 対象のファイルが新規作成なのか？それとも既存ファイルのロードなのか

	_file = filePath;
	_systemPageSize = sysconf( _SC_PAGESIZE ); // ランタイムで取得する
	_fd = open( filePath.c_str() , O_RDWR | O_CREAT, (mode_t)(0600) );

	if( _fd < 0 ) return;
	if( stat( filePath.c_str() , &_st ) != 0 ) return;

	off_t currentFileSize = _st.st_size;
	if( currentFileSize < BLK_REV_META_BLOCK_SIZE ){
		ftruncate( _fd , BLK_REV_META_BLOCK_SIZE ); // 領域が不足していたら確保する
		isNewFile = true;
	}

	std::cout << "Open " << filePath << " with FD :: " << _fd << "\n";

	// Meta領域の確保とマッピング
	 _mapping._addr = mmap( NULL , BLK_REV_META_BLOCK_SIZE , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , 0 );
	_meta = (struct LocalFileController::Meta*)(_mapping._addr);

	if( isNewFile )
		_meta->actualDataSize( BLK_REV_META_BLOCK_SIZE );

	std::cout << "## データ最後尾 :: " << _meta->actualDataSize() << "\n";
}

void LocalFileController::Meta::actualDataSize( size_t target )
{
	_actualDataSize =  static_cast<uint32_t>(target);
	msync( this , BLK_REV_META_BLOCK_SIZE , MS_SYNC ); // このthisはLocalFileController._metaを参照する？
}

size_t LocalFileController::Meta::actualDataSize()
{
	return static_cast<size_t>(_actualDataSize );
}

long LocalFileController::write( std::shared_ptr<unsigned char> data , size_t dataLength )
{
	// データの最後尾はどこか
	size_t dataEnd = _meta->actualDataSize(); // データ最後尾までのバイト数を取得する

	if( dataEnd + dataLength >= DEFAULT_BLK_REV_MAX_BYTES ) return -1; // 次のファイルに書き込め命令
	ftruncate( _fd , dataEnd + dataLength ); // ファイルサイズの拡張
	off_t offset = ( dataEnd / _systemPageSize ) /* ページサイズ何個分見送るか*/;
	off_t mappingInOffset = dataEnd - (_systemPageSize * offset); // 

	_mapping._addr = mmap( NULL , mappingInOffset + dataLength , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , (offset * _systemPageSize) );
	_mapping._size = mappingInOffset + dataLength;

	memcpy( (static_cast<unsigned char *>(_mapping._addr) + mappingInOffset) , data.get() , dataLength );
	
	munmap( _mapping._addr , _mapping._size ); // 簡単にマップ・アンマップしないほうがいいのかは不明
	_meta->actualDataSize( dataEnd + dataLength ); // 最後尾を設定

	return dataEnd;
}


size_t LocalFileController::read( std::shared_ptr<unsigned char> *data, size_t size,  off_t offset )
{
	off_t offsetByPageSize = offset / _systemPageSize; // ページサイズ何個分見送るか
	off_t mappingInOffset = offset - (_systemPageSize * offsetByPageSize);

	*data = std::shared_ptr<unsigned char>( new unsigned char[size] );

	size_t currentPtr = 0;
	_mapping._addr = mmap( NULL , size + mappingInOffset , PROT_READ | PROT_WRITE , MAP_SHARED , _fd , offsetByPageSize * _systemPageSize  );
	_mapping._size = size + mappingInOffset;
	memcpy( (*data).get() , (static_cast<unsigned char*>(_mapping._addr) + mappingInOffset) , size ); // Metaの取り込み
	currentPtr += size; // 実際に読み込んだのはこのサイズ
	munmap( _mapping._addr , _mapping._size ); // 一旦アンマップする

	return currentPtr;
}


long BlkFileManager::write( std::shared_ptr<BlockContainer> container )
{
	std::shared_ptr<unsigned char> rawBlockContainer; size_t rawBlockContainerLength;
	rawBlockContainerLength = container->exportRawFormated( &rawBlockContainer );  // ブロックコンテナの書き出し

	return LocalFileController::write( rawBlockContainer , rawBlockContainerLength ); // 書き込んだ先頭位置を返却する
}


std::shared_ptr<BlockContainer> BlkFileManager::read( off_t offset )
{
	// 1. BlockContainer Metaの読み込み
	// 2. ブロック本体(バイナリ)の読み込み
	// 3. ブロックの読み込み

	struct BlockContainer container;

	// ブロックコンテナMetaの読み込み
	std::shared_ptr<unsigned char> readedContainerMeta; size_t readedContainerMetaLength;
	readedContainerMetaLength = LocalFileController::read( &readedContainerMeta , sizeof(container._meta) , offset );
	container.importMetaSequentially( readedContainerMeta );

	// ブロックコンテナ保体の読み込み
	std::shared_ptr<unsigned char> readedContainer; size_t readedContainerLength;
	// Meta分ずらして取得する
	readedContainerLength = LocalFileController::read( &readedContainer, container.size() - sizeof(container._meta) , offset + sizeof(container._meta) );

	container.importBlockSequentially( readedContainer );

	return std::make_shared<BlockContainer>( container );
}

long RevFileManager::write( std::shared_ptr<Block> targetBlock )
{
	struct UndoContainer undoContainer( targetBlock );
	std::shared_ptr<unsigned char> rawUNDOContainer; size_t rawUNDOContainerLength;
	rawUNDOContainerLength = undoContainer.exportRawFormated( &rawUNDOContainer );
	
	return LocalFileController::write( rawUNDOContainer , rawUNDOContainerLength );
}

std::shared_ptr<UndoContainer> RevFileManager::read( off_t offset )
{
	struct UndoContainer container;

	std::shared_ptr<unsigned char> readedContainerMeta; size_t readedContainerMetaLength;
	readedContainerMetaLength = LocalFileController::read( &readedContainerMeta , sizeof(container._meta) , offset );
	container.importMetaSequentially( readedContainerMeta );


	std::shared_ptr<unsigned char> readedContainer; size_t readedContainerLength;
	readedContainerLength = LocalFileController::read( &readedContainer , container.size() - sizeof(container._meta) , offset + sizeof(container._meta) );
	container.importUndoFromRaw( readedContainer , readedContainerLength );

	return std::make_shared<UndoContainer>(container);
}

std::string LocalFileController::file()
{
	return _file;
}


LocalFileController::~LocalFileController()
{
	munmap( _meta , BLK_REV_META_BLOCK_SIZE );
	munmap( _mapping._addr , _mapping._size );
	close( _fd );
}


};
