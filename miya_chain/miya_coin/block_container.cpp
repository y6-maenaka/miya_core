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

std::shared_ptr<block::Block> BlockContainer::block()
{
		return _block;
}



void BlockContainer::importMetaSequentially( std::shared_ptr<unsigned char> fromRaw )
{
	// SIGSEGはハンドリングするようにする
	memcpy( &_meta , fromRaw.get() , sizeof(_meta) );
}

void BlockContainer::importBlockSequentially( std::shared_ptr<unsigned char> fromRaw )
{
  size_t currentPtr = 0;

  // ブロックヘッダの取り込み
	currentPtr += _block->header()->importRawSequentially( fromRaw.get() + currentPtr );

	// coinbaseの取り込み
	std::shared_ptr<tx::Coinbase> readedCoinbase = std::make_shared<tx::Coinbase>();
	currentPtr += readedCoinbase->importRawSequentially( fromRaw.get() + currentPtr );
	_block->coinbase( readedCoinbase );

	// トランザクションの取り込み
	std::cout << txCount()	 << "\n";
	for( int i=0; i<txCount(); i++ ){
		std::shared_ptr<tx::P2PKH> readedTx = std::make_shared<tx::P2PKH>();
		currentPtr += readedTx->importRawSequentially( fromRaw.get() + currentPtr );
		_block->add( readedTx );
	}

	std::cout << "importBLock currentPtr :: " << currentPtr << "\n";
  return;
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

	size_t retLength = sizeof(_meta) + rawBlockHeaderLength + rawTxsLength + rawCoinbaseLength; 
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








}
