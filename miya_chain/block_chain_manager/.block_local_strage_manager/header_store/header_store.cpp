#include "header_store.h"





namespace miya_chain
{

HeaderStore::HeaderStore( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer ) : MiyaDBSBClient( pushSBContainer , popSBContainer )
{
	return;
}




std::shared_ptr<block::BlockHeader> HeaderStore::get( std::shared_ptr<unsigned char> blockHash )
{
	std::shared_ptr<unsigned char> rawBlockHeader; size_t rawBlockHeaderLength;
	rawBlockHeaderLength = MiyaDBSBClient::get( blockHash , &rawBlockHeader );

	if( rawBlockHeader == nullptr ) return nullptr;

	std::shared_ptr<block::BlockHeader> retHeader = std::make_shared<block::BlockHeader>();
	retHeader->importRaw( rawBlockHeader , rawBlockHeaderLength );

	return retHeader;
}



bool HeaderStore::add( std::shared_ptr<block::Block> block )
{
	return this->add( block->header() );
}



bool HeaderStore::add( block::BlockHeader *blockHeader )
{

	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKeyLength = blockHeader->headerHash( &rawKey ); // ヘッダはっしゅをキーとする

	std::shared_ptr<unsigned char> rawValue; size_t rawValueLength;
	rawValueLength = blockHeader->exportRaw( &rawValue );

	return MiyaDBSBClient::add( rawKey , rawValue, rawValueLength );
}





};
