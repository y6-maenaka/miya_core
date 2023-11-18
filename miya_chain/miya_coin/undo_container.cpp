#include "./local_strage_manager.h"


#include "../block/block.h"
#include "../transaction/coinbase/coinbase.h"
#include "../transaction/p2pkh/p2pkh.h"
#include "../utxo_set/utxo.h"
#include "../../shared_components/json.hpp"




namespace miya_chain
{






UndoContainer::UndoContainer()
{
	return;
}


UndoContainer::UndoContainer( std::shared_ptr<block::Block> blockFrom )
{
	size_t outputIndex = 0;

	// coinbaseのundoを作成
	std::shared_ptr<unsigned char> coinbaseTxID;
	blockFrom->coinbase()->calcTxID( &coinbaseTxID );
	std::shared_ptr<UTXO> coinbaseUTXO = std::make_shared<UTXO>( blockFrom->coinbase()->txOut() , coinbaseTxID ,static_cast<uint32_t>(outputIndex) ); // コインベースのインデックスは0
	outputIndex += 1;
	_utxoVector.push_back( coinbaseUTXO );


	// ブロックに含まれる通常トランザクションのundoを作成
	std::shared_ptr<unsigned char>	txID;
	std::vector< std::shared_ptr<tx::P2PKH> > txVector = blockFrom->txVector();
	for( auto txItr : txVector )
	{
		txItr->calcTxID( &txID );
		for( auto txOutItr : txItr->outs() )
		{
			std::shared_ptr<UTXO>	txTxOut = std::make_shared<UTXO>( txOutItr , txID ,static_cast<uint32_t>(outputIndex) );
			outputIndex += 1;
			_utxoVector.push_back( txTxOut );
		}
	}


	_meta._count = static_cast<uint32_t>( _utxoVector.size() );

	std::cout << "undo count :: " << count() << "\n";

	return;
}


size_t UndoContainer::size()
{
		return static_cast<size_t>( _meta._size );
}

size_t UndoContainer::count()
{
		return static_cast<size_t>( _meta._count );
}


std::vector< std::shared_ptr<miya_chain::UTXO> > UndoContainer::utxoVector()
{
	return _utxoVector;
}


void UndoContainer::importMetaSequentially( std::shared_ptr<unsigned char> fromRaw )
{
	memcpy( &_meta , fromRaw.get() , sizeof(_meta) );
}


void UndoContainer::importUndoFromRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	std::vector<uint8_t> fromRawVector;
	fromRawVector.assign( fromRaw.get() , fromRaw.get() + fromRawLength );
	
	std::cout << "## 1" << "\n";


	nlohmann::json dumpedUTXOBatch;
	dumpedUTXOBatch = nlohmann::json::from_bson( fromRawVector );

	std::cout << "## 2 " << "\n";

	for( auto itr : dumpedUTXOBatch["dumpedUTXOs"] )
	{
		if( !(itr.is_binary()) ) continue;
		std::shared_ptr<UTXO> importedUTXO = std::make_shared<UTXO>();
		if( importedUTXO->importFromBson( itr ) ){
			_utxoVector.push_back( importedUTXO );
		}
	}

	_meta._count = static_cast<uint32_t>( _utxoVector.size() );
	std::cout << "readed :: " << _utxoVector.size() << "\n";

	return;
}



size_t UndoContainer::exportRawFormated( std::shared_ptr<unsigned char> *retRaw )
{
	nlohmann::json dumpedUTXOBatch;
	for( auto itr : _utxoVector )
		dumpedUTXOBatch["dumpedUTXOs"].push_back( nlohmann::json::binary( itr->dumpToBson() ));

	std::vector<uint8_t> dumpedUTXOBatchVector;
	dumpedUTXOBatchVector = nlohmann::json::to_bson( dumpedUTXOBatch );


	size_t retRawLength = sizeof(_meta) + dumpedUTXOBatchVector.size();
	// 書き出しサイズの指定
	_meta._size = static_cast<uint32_t>( retRawLength );

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );
	size_t formatPtr = 0;

	// Meta領域のコピー
	memcpy( (*retRaw).get() + formatPtr , &_meta , sizeof(_meta) );
	formatPtr += sizeof(_meta);

	// UTXOのコピー
	std::copy( dumpedUTXOBatchVector.begin() , dumpedUTXOBatchVector.end() , (*retRaw).get() + formatPtr );
	formatPtr += dumpedUTXOBatchVector.size();

	return formatPtr;
}





};
