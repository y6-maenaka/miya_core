#include "local_strage_manager.h" 

#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"
#include "../transaction/p2pkh/p2pkh.h"

#include "../../shared_components/json.hpp"
#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../shared_components/miya_db_client/common.h"

#include "../../shared_components/miya_db_client/miya_db_sb_client.h"


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
		std::cout << "exportedTx Length :: " << rawTxLength << "\n";
	}
	
	_meta._txCount = static_cast<uint32_t>(rawTxVector.size());

	
	size_t retLength = sizeof(_meta) + rawBlockHeaderLength + rawTxsLength;
	_meta._size = static_cast<uint32_t>(retLength); 

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
	
	size_t formatPtr = 0;
	memcpy( (*retRaw).get() + formatPtr , &_meta , sizeof(_meta) );  formatPtr += sizeof(_meta); // metaの書き出し
	memcpy( (*retRaw).get() + formatPtr , rawBlockHeader.get() , rawBlockHeaderLength ); formatPtr += rawBlockHeaderLength; // ブロックヘッダの書き出し
	memcpy( (*retRaw).get() + formatPtr , rawCoinbase.get() , rawCoinbaseLength ); formatPtr += rawCoinbaseLength;
	for( auto itr : rawTxVector ) {  // トランザクションの書き出し
		memcpy( (*retRaw).get() + formatPtr, (itr.first).get() , itr.second );  
		formatPtr += itr.second;
	}

	return formatPtr;
}










// ## ファイルがひとつもない場合はエラーが発生する(ジェネシスブロックを必ず書き込む)
BlockLocalStrageManager::BlockLocalStrageManager( std::shared_ptr<StreamBufferContainer> toIndexDBSBC , std::shared_ptr<StreamBufferContainer> fromIndexDBSBC )
{
	_miyaDBClient = std::make_shared<MiyaDBSBClient>( toIndexDBSBC , fromIndexDBSBC );
	_blkFileManager = std::make_shared<BlkFileManager>( "" );
	_revFileManager = std::make_shared<RevFileManager>( "" );


	/* ------------ blk/rev最後のファイルインデックスを取得する ------------ */
	std::regex re(R"(blk(\d+)\.dat)");
	std::cmatch m;
	std::string pathToBlocksDirectory = "../miya_chain/miya_coin/blocks/";
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








void BlockLocalStrageManager::writeBlock( std::shared_ptr<block::Block> targetBlock )
{
	std::shared_ptr<BlockContainer> container = std::make_shared<BlockContainer>(targetBlock); // pack block to blockContainer

	std::string filePath = "../miya_chain/miya_coin/blocks/" + std::string("blk") + std::to_string(_lastIndex) + ".dat";
	std::cout << "write to >> " << filePath << "\n";

		// キャッシュの確認
	long offset;	
	if( _blkFileManager->file() == filePath ) // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		offset = _blkFileManager->write( container );
	}
	else // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager(filePath) );
		offset = _blkFileManager->write( container );
	}

	if( offset == -1  ) // 新規にblkファイルを作成する必要がある
	{
		_lastIndex += 1;
		filePath = "../miya_chain/miya_coin/blocks/" + std::string("blk") + std::to_string(_lastIndex) + ".dat";
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager(filePath) );
		offset = _blkFileManager->write( container );
	}
	
	// インデックスの登録 
	std::shared_ptr<unsigned char> blockHash = std::shared_ptr<unsigned char>( new unsigned char[32] ); 
	targetBlock->blockHash( &blockHash );

	BlockIndex blockIndex; 
	blockIndex.fileIndex = static_cast<uint16_t>(0); // あとで修正
	blockIndex.blockOffset = static_cast<uint32_t>(offset);
	blockIndex.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::shared_ptr<unsigned char> rawBlockIndex; size_t rawBlockIndexLength;
	rawBlockIndexLength = blockIndex.exportRaw( &rawBlockIndex );

	_miyaDBClient->add( blockHash , rawBlockIndex , rawBlockIndexLength );

	std::cout << "Block Writed" << "\n";

	return;
}



std::shared_ptr<block::Block> BlockLocalStrageManager::readBlock( std::shared_ptr<unsigned char> blockHash )
{
	std::shared_ptr<unsigned char> queryRetRaw; size_t queryRetRawLength;
	queryRetRawLength = _miyaDBClient->get( blockHash , &queryRetRaw );

	if( queryRetRaw == nullptr || queryRetRawLength <= 0 ) return nullptr;

	// queryRetRawをBlockIndexに変換する
	struct BlockIndex blockIndex;
	memcpy( &blockIndex , queryRetRaw.get() , sizeof(blockIndex) );

	std::string filePath = "../miya_chain/miya_coin/blocks/" + std::string("blk") + std::to_string(blockIndex.fileIndex) + ".dat";
	std::cout << "read from" << filePath << "\n";


	std::shared_ptr<BlockContainer> container;
	off_t offset = blockIndex.blockOffset;
	if( _blkFileManager->file() == filePath ) // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		container = _blkFileManager->read( offset );
	}
	else // 前回使用した(キャッシュ)ファイルマネージャーの管理ファイルが異なる場合
	{
		std::string filePath = "../miya_chain/miya_coin/blocks/" + std::string("blk") + std::to_string(blockIndex.fileIndex) + ".dat";
		_blkFileManager = std::shared_ptr<BlkFileManager>( new BlkFileManager(filePath) );
		container = _blkFileManager->read( offset );
	}


	if( container == nullptr ) return nullptr;
	std::cout << "Block Readed From Blk" << "\n";
	return container->_block;
}





std::shared_ptr<tx::P2PKH> BlockLocalStrageManager::readTx( std::shared_ptr<unsigned char> txHash )
{

}




}
