#include "value_store_manager.h"


#include "../page_table/overlay_ptr.h"
#include "../page_table/optr_utils.h"
#include "../page_table/overlay_memory_manager.h"

#include "../../../../miya_db/query_context/query_context.h"


namespace miya_db
{





ValueFragmentHeader::ValueFragmentHeader()
{
	//std::time_t timestamp = std::time(nullptr);
	uint64_t timestamp = static_cast<uint64_t>( std::time(nullptr) );
	_meta._timestamp = timestamp;


	std::shared_ptr<unsigned char> addrZero = std::shared_ptr<unsigned char>( new unsigned char[sizeof(addrZero)] );  memcpy( addrZero.get() , 0x00 , sizeof(addrZero) );
	_meta._prevFlagment = std::shared_ptr<optr>( new optr(addrZero.get()) );
	_meta._nextFlagment = std::shared_ptr<optr>( new optr(addrZero.get()) );

	_meta._valueLength = 0;
}




void ValueFragmentHeader::valueLength( size_t valueLength )
{
	_meta._valueLength = static_cast<uint64_t>(valueLength);
}



size_t ValueFragmentHeader::valueLength()
{
	return static_cast<size_t>( _meta._valueLength );
}



size_t ValueFragmentHeader::headerLength()
{
	return static_cast<size_t>(_meta._headerLength);
}



void ValueFragmentHeader::importBinary( std::shared_ptr<optr> startFlagmentOptr )
{
	int currentPtr = 0;


	omemcpy( &(_meta._headerLength) , startFlagmentOptr.get() , sizeof(_meta._headerLength) ); // リスト構造のため不要かも
	currentPtr += sizeof(_meta._headerLength);

	
	std::shared_ptr<unsigned char> prevOAddr = std::shared_ptr<unsigned char>( new unsigned char[VALUE_FLAGMENT_ADDR_SIZE] );
	omemcpy( prevOAddr.get() , *startFlagmentOptr + currentPtr  ,  VALUE_FLAGMENT_ADDR_SIZE );
	_meta._prevFlagment->addr( prevOAddr.get() );
	currentPtr += VALUE_FLAGMENT_ADDR_SIZE;

	std::shared_ptr<unsigned char> nextOAddr = std::shared_ptr<unsigned char>( new unsigned char[VALUE_FLAGMENT_ADDR_SIZE] );
	omemcpy( nextOAddr.get() , *startFlagmentOptr +  currentPtr ,  VALUE_FLAGMENT_ADDR_SIZE );
	_meta._nextFlagment->addr( nextOAddr.get() );
	currentPtr += VALUE_FLAGMENT_ADDR_SIZE;

	omemcpy( &(_meta._valueLength) , *startFlagmentOptr + currentPtr , sizeof(_meta._valueLength) );
	currentPtr += sizeof(_meta._valueLength);

	omemcpy( &(_meta._timestamp) , *startFlagmentOptr + currentPtr , sizeof(_meta._timestamp) );
	currentPtr += sizeof(_meta._valueLength);

	return;
}



size_t ValueFragmentHeader::exportBianry( std::shared_ptr<unsigned char> *ret )
{
	*ret = std::shared_ptr<unsigned char>( new unsigned char[VALUE_FLAGMENT_ADDR_SIZE] );

	_meta._headerLength = DEFAULT_VALUE_FLAGMENT_HEADER_SIZE;

	int formatPtr = 0;
	memcpy( (*ret).get() , &(_meta._headerLength)  , sizeof(_meta._headerLength) ); formatPtr += sizeof(_meta._headerLength);

	memcpy( (*ret).get() + formatPtr , _meta._prevFlagment->addr() , sizeof(VALUE_FLAGMENT_ADDR_SIZE) ); formatPtr += sizeof(VALUE_FLAGMENT_ADDR_SIZE);
	memcpy( (*ret).get() + formatPtr , _meta._nextFlagment->addr() , sizeof(VALUE_FLAGMENT_ADDR_SIZE) ); formatPtr += sizeof(VALUE_FLAGMENT_ADDR_SIZE);

	memcpy( (*ret).get() + formatPtr , &(_meta._valueLength) , sizeof(_meta._valueLength) ); formatPtr += sizeof(_meta._valueLength);

	memcpy( (*ret).get() + formatPtr , &(_meta._timestamp) , sizeof(_meta._timestamp) ); formatPtr += sizeof(_meta._timestamp);

	return formatPtr;
}




















ValueStoreManager::ValueStoreManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_dataOverlayMemoryManager = oMemoryManager;
}






std::shared_ptr<optr> ValueStoreManager::add( std::shared_ptr<QueryContext> qctx )
{
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>();
	flagmentHeader->valueLength( qctx->valueLength() );

	std::shared_ptr<unsigned char> exportedFlagmentHeader;
	size_t exportedFlagmentHeaderLength = flagmentHeader->exportBianry( &exportedFlagmentHeader );

	std::shared_ptr<optr> storeTargetOptr;
	storeTargetOptr = _dataOverlayMemoryManager->allocate( exportedFlagmentHeaderLength + qctx->valueLength() );

	omemcpy( storeTargetOptr.get() , flagmentHeader.get() , flagmentHeader->headerLength());
	omemcpy( (*storeTargetOptr + flagmentHeader->headerLength()).get() , qctx->value().get() , qctx->valueLength() );

	return storeTargetOptr;
}





size_t ValueStoreManager::get( std::shared_ptr<optr> targetOptr ,std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>();
	_dataOverlayMemoryManager->wrap( targetOptr ); // キャッシュテーブルを事前にセットしなければならない
	flagmentHeader->importBinary( targetOptr );

	// 本来はここで他フラグメントが存在する場合は,それに合わせて拡張する


	size_t valueLength = flagmentHeader->valueLength(); 
	*ret = std::shared_ptr<unsigned char>( new unsigned char[valueLength] );
	omemcpy( (*ret).get() , *targetOptr + (flagmentHeader->headerLength()) , flagmentHeader->valueLength() );

	return valueLength;
}





};
