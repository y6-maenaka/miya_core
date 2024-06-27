#include "local_strage_manager.h"

#include <chain/transaction/coinbase/coinbase.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/block/block.h>
#include <chain/block/block.hpp>
#include <chain/utxo_set/utxo.h>
#include <chain/utxo_set/utxo_set.h>

#include <json.hpp>
#include <stream_buffer/stream_buffer.h>
#include <stream_buffer/stream_buffer_container.h>
#include <miya_db_client/common.h>
#include <miya_db_client/miya_db_sb_client.h>


namespace chain
{


BlockIndex::BlockIndex()
{
	// ゼロ埋め
	memset(this, 0x00 , sizeof(struct BlockIndex));
}

size_t BlockIndex::blkOffset()
{
	return static_cast<size_t>(blockOffset);
}

size_t BlockIndex::revOffset()
{
	return static_cast<size_t>(reversalOffset);
}



size_t BlockIndex::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
		*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct BlockIndex)] );
		memcpy( (*retRaw).get() , this , sizeof(struct BlockIndex) );
		return sizeof(struct BlockIndex);
}

void BlockIndex::importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	if( fromRaw == nullptr || fromRawLength < sizeof(struct BlockIndex ) ) return;

	memcpy( this , fromRaw.get() , sizeof( struct BlockIndex) );
}



// ## ファイルがひとつもない場合はエラーが発生する(ジェネシスブロックを必ず書き込む)
BlockLocalStrage::BlockLocalStrage( std::shared_ptr<StreamBufferContainer> toIndexDBSBC , std::shared_ptr<StreamBufferContainer> fromIndexDBSBC )
{
	_miyaDBClient = std::make_shared<MiyaDBSBClient>( toIndexDBSBC , fromIndexDBSBC );
	_blkFileManager = std::make_shared<BlkFileManager>( "" );
	_revFileManager = std::make_shared<RevFileManager>( "" );

	/* ------------ blk/rev最後のファイルインデックスを取得する ------------ */
	std::regex re(R"(blk(\d+)\.dat)");
	std::cmatch m;
	std::string pathToBlocksDirectory = "../chain/miya_coin/blocks/";
	std::set<unsigned int> indexSet;
	for( const auto & entry : std::filesystem::directory_iterator(pathToBlocksDirectory) )
	{
		if( entry.is_regular_file() ){
			if( std::regex_match( entry.path().filename().string().c_str(), m , re ) ){
				indexSet.insert( static_cast<unsigned int>(std::stoi(m[1])) );
				_lastIndex = std::max( static_cast<unsigned int>(_lastIndex) , static_cast<unsigned int>(std::stoi(m[1])) );
			}
		}
	}

	std::cout << "<LocalStrageManager> LastIndex :: " << _lastIndex << "\n";
	std::cout << "<LocalStrageManager> IndexSize :: " << indexSet.size() - 1<< "\n";

	assert( static_cast<unsigned int>(_lastIndex) == static_cast<unsigned int>(indexSet.size() - 1) );

}


std::vector< std::shared_ptr<UTXO> > BlockLocalStrage::writeBlock( std::shared_ptr<Block> targetBlock )
{
	std::shared_ptr<BlockContainer> container = std::make_shared<BlockContainer>(targetBlock); // pack block to blockContainer

	std::string blkFilePath = "../chain/miya_coin/blocks/" + std::string("blk") + std::to_string(_lastIndex) + ".dat";
	std::string revFilePath = "../chain/miya_coin/blocks/" + std::string("rev") + std::to_string(_lastIndex) + ".dat";

		// キャッシュの確認
	long blkOffset;
	long revOffset;
	if( _blkFileManager->file() == blkFilePath ) // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		blkOffset = _blkFileManager->write( container );
		revOffset = _revFileManager->write( container->block() ); 
	}
	else // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager(blkFilePath) );
		_revFileManager = std::shared_ptr<RevFileManager>( new RevFileManager(revFilePath) );
		blkOffset = _blkFileManager->write( container );
		revOffset = _revFileManager->write( container->block() ); 
	}

	if( blkOffset == -1 || revOffset == -1 ) // 新規にblkファイルを作成する必要がある ( 切り替えのベースは基本的にブロックファイルが満帆になった時 )
	{
		std::cout << "新規にblk,revファイルを作成します" << "\n";
		_lastIndex += 1;
		blkFilePath = "../chain/miya_coin/blocks/" + std::string("blk") + std::to_string(_lastIndex) + ".dat";
		revFilePath = "../chain/miya_coin/blocks/" + std::string("rev") + std::to_string(_lastIndex) + ".dat";
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager(blkFilePath) );
		_revFileManager = std::shared_ptr<RevFileManager>( new RevFileManager(revFilePath) );
		blkOffset = _blkFileManager->write( container );
		revOffset = _revFileManager->write( container->block() );
	}

	// インデックスの登録
	std::shared_ptr<unsigned char> blockHash = std::shared_ptr<unsigned char>( new unsigned char[32] );
	targetBlock->blockHash( &blockHash );
	
	BlockIndex blockIndex;
	blockIndex.fileIndex = static_cast<uint16_t>(0); // あとで修正
	blockIndex.blockOffset = static_cast<uint32_t>(blkOffset);
	blockIndex.reversalOffset = static_cast<uint32_t>(revOffset);
	blockIndex.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::shared_ptr<unsigned char> rawBlockIndex; size_t rawBlockIndexLength;
	rawBlockIndexLength = blockIndex.exportRaw( &rawBlockIndex );

	// 保存情報をデータベースにストアしておく
	_miyaDBClient->add( blockHash , rawBlockIndex , rawBlockIndexLength );

	std::cout << "Block Writed" << "\n";
	std::cout << "Rev Writed" << "\n";

	UndoContainer writedUndoContainer( container->block() );
	return writedUndoContainer.utxoVector();
}


std::shared_ptr<Block> BlockLocalStrage::readBlock( std::shared_ptr<unsigned char> blockHash )
{
	std::shared_ptr<unsigned char> queryRetRaw; size_t queryRetRawLength;
	queryRetRawLength = _miyaDBClient->get( blockHash , &queryRetRaw );

	if( queryRetRaw == nullptr || queryRetRawLength <= 0 ) return nullptr;

	// queryRetRawをBlockIndexに変換する
	struct BlockIndex blockIndex;
	memcpy( &blockIndex , queryRetRaw.get() , sizeof(blockIndex) );

	std::string blkFilePath = "../chain/miya_coin/blocks/" + std::string("blk") + std::to_string(blockIndex.fileIndex) + ".dat";
	std::string revFilePath = "../chain/miya_coin/blocks/" + std::string("rev") + std::to_string(blockIndex.fileIndex) + ".dat";

	std::cout << "read from" << blkFilePath << "\n";

	std::shared_ptr<BlockContainer> container;
	off_t offset = blockIndex.blockOffset;
	if( _blkFileManager->file() == blkFilePath ) // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		container = _blkFileManager->read( offset );
	}
	else // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager( blkFilePath) );
		_revFileManager = std::shared_ptr<RevFileManager>( new RevFileManager(revFilePath) ); // 使わないがキャッシュ用に作成しておく
		container = _blkFileManager->read( offset );
	}


	if( container == nullptr ) return nullptr;
	std::cout << "Block Readed From Blk" << "\n";
	return container->_block;
}


std::shared_ptr<struct Block> BlockLocalStrage::read_block( std::array<std::uint8_t, 256/8>  bh )
{
  return nullptr;
}


std::vector< std::shared_ptr<UTXO> > BlockLocalStrage::readUndo( std::shared_ptr<unsigned char> blockHash )
{
	std::shared_ptr<unsigned char> queryRetRaw; size_t queryRetRawLength;
	queryRetRawLength = _miyaDBClient->get( blockHash , &queryRetRaw );

	if( queryRetRaw == nullptr || queryRetRawLength <= 0 ) return std::vector< std::shared_ptr<UTXO> >();

	// queryRetRawをBlockIndexに変換する
	struct BlockIndex blockIndex;
	memcpy( &blockIndex , queryRetRaw.get() , sizeof(blockIndex) );

	std::string filePath = "../chain/miya_coin/blocks/" + std::string("rev") + std::to_string(blockIndex.fileIndex) + ".dat";

	std::shared_ptr<UndoContainer> container;
	off_t offset = blockIndex.revOffset();
	if( _revFileManager->file() == filePath ) // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		std::cout << "RevRead キャッシュが存在します" << "\n";
		container = _revFileManager->read( offset );
	}
	else // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		std::cout << "新規に作成します" << "\n";
		std::string filePath = "../chain/miya_coin/blocks/" + std::string("rev") + std::to_string(blockIndex.fileIndex) + ".dat";
		_revFileManager = std::shared_ptr<RevFileManager>( new RevFileManager(filePath) );
		container = _revFileManager->read( offset );
	}

	if( container == nullptr ) return std::vector< std::shared_ptr<UTXO> >();

	return container->utxoVector();
}


std::vector< std::shared_ptr<UTXO> > BlockLocalStrage::releaseBlock( std::shared_ptr<unsigned char> blockHash )
{
	std::vector< std::shared_ptr<UTXO> > utxoVector = this->readUndo( blockHash );

	_miyaDBClient->remove( blockHash );  // 現実装ではデータ削除は行わない将来的には書き込んでいるデータを削除するようにする

	std::cout << "Block Released" << "\n";
	return utxoVector;
}


}
