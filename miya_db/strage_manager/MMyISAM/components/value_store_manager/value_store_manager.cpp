#include "value_store_manager.h"


#include "../page_table/overlay_ptr.h"
#include "../page_table/optr_utils.h"
#include "../page_table/overlay_memory_manager.h"
#include "../page_table/cache_manager/cache_table.h"

#include "../../../../miya_db/query_context/query_context.h"


namespace miya_db
{





ValueFragmentHeader::ValueFragmentHeader( std::shared_ptr<CacheTable> cacheTable )
{
	std::cout << "ValueFragmentHeader Initalizing" << "\n";

	std::cout << "--- check 1 ---" << "\n";

	//std::time_t timestamp = std::time(nullptr);
	uint64_t timestamp = static_cast<uint64_t>( std::time(nullptr) );
	_meta._timestamp = timestamp;

	std::cout << "--- check 2 ---" << "\n";

	std::shared_ptr<unsigned char> addrZero = std::shared_ptr<unsigned char>( new unsigned char[VALUE_FLAGMENT_ADDR_SIZE] ); 
	memset( addrZero.get() , 0x00 , VALUE_FLAGMENT_ADDR_SIZE );

	std::cout << "--- check 3 ---" << "\n";

	_meta._prevFlagment = std::shared_ptr<optr>( new optr(addrZero.get(), cacheTable ) );

	std::cout << "--- check 4 ---" << "\n";

	_meta._nextFlagment = std::shared_ptr<optr>( new optr(addrZero.get(), cacheTable ) );

	_meta._valueLength = 0;

	std::cout << " <-<- りたーんど ->->" << "\n";
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
	*ret = std::shared_ptr<unsigned char>( new unsigned char[DEFAULT_VALUE_FLAGMENT_HEADER_SIZE] );
	_meta._headerLength = DEFAULT_VALUE_FLAGMENT_HEADER_SIZE;


	std::cout << "------------------------------------------------" << "\n";
	std::cout << sizeof(_meta._headerLength) << " HeaderLength :: " << _meta._headerLength << "\n";
	std::cout << 5 << " PrevFlagment :: "; _meta._prevFlagment->printAddr(); std::cout << "\n";
	std::cout << 5 << " NextFlagment :: "; _meta._nextFlagment->printAddr(); std::cout << "\n";
	std::cout << sizeof(_meta._valueLength) << " ValueLength :: " << _meta._valueLength << "\n";
	std::cout << sizeof(_meta._timestamp) << " TiemStamp :: " << _meta._timestamp << "\n";
	std::cout << "------------------------------------------------" << "\n";


	std::cout << "** check 1" << "\n";

	int formatPtr = 0;
	memcpy( (*ret).get() , &(_meta._headerLength)  , sizeof(_meta._headerLength) ); formatPtr += sizeof(_meta._headerLength);

	std::cout << "formatPtr(1) -> " << formatPtr << "\n";

	omemcpy( (*ret).get() + formatPtr , _meta._prevFlagment.get() , VALUE_FLAGMENT_ADDR_SIZE ); formatPtr += VALUE_FLAGMENT_ADDR_SIZE;

	std::cout << "** check 3" << "\n";
	std::cout << "formatPtr(2) -> " << formatPtr << "\n";

	omemcpy( (*ret).get() + formatPtr , _meta._nextFlagment.get() , VALUE_FLAGMENT_ADDR_SIZE ); formatPtr += VALUE_FLAGMENT_ADDR_SIZE;

	std::cout << "** check 4" << "\n";
	std::cout << "formatPtr(3) -> " << formatPtr << "\n";

	memcpy( (*ret).get() + formatPtr , &(_meta._valueLength) , sizeof(_meta._valueLength) ); formatPtr += sizeof(_meta._valueLength);
	
	std::cout << "** check 5" << "\n";
	std::cout << "formatPtr(4) -> " << formatPtr << "\n";

	memcpy( (*ret).get() + formatPtr , &(_meta._timestamp) , sizeof(_meta._timestamp) ); formatPtr += sizeof(_meta._timestamp);

	std::cout << "** check 6" << "\n";
	std::cout << "formatPtr(5) -> " << formatPtr << "\n";


	std::cout << "--------------------------------" << "\n";
	std::cout << "exportBinary formatPtr -> " << formatPtr << "\n";
	std::cout << "--------------------------------" << "\n";

	return formatPtr;
}




















ValueStoreManager::ValueStoreManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_dataOverlayMemoryManager = oMemoryManager;
}






std::shared_ptr<optr> ValueStoreManager::add( std::shared_ptr<QueryContext> qctx )
{
	std::cout << "ValueStoreManager::add() called" << "\n";


	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>(
		_dataOverlayMemoryManager->dataCacheTable()
	);


	std::cout << " < check 0 " << "\n";
	flagmentHeader->valueLength( qctx->valueLength() );

	std::cout << "< check 1 " << "\n";

	std::shared_ptr<unsigned char> exportedFlagmentHeader;
	size_t exportedFlagmentHeaderLength = flagmentHeader->exportBianry( &exportedFlagmentHeader );

	std::cout << "< check 2 " << "\n";

	std::cout << "--------------------------------" << "\n";
	for( int i=0; i<exportedFlagmentHeaderLength; i++ )
	{
		printf("%02X", exportedFlagmentHeader.get()[i] );
	} std::cout << "\n";
	std::cout << "flagmentHeaderLength -> " << flagmentHeader->headerLength() << "\n";
	std::cout << "exportedFlagmentHeaderLenght -> " << exportedFlagmentHeaderLength << "\n";
	std::cout << "qctx->valueLength() -> " << qctx->valueLength() << "\n";
	std::cout << "--------------------------------" << "\n";


	std::cout << "< check 3 " << "\n";

	std::shared_ptr<optr> storeTargetOptr;
	storeTargetOptr = _dataOverlayMemoryManager->allocate( exportedFlagmentHeaderLength + qctx->valueLength() );

	std::cout << "---------------------" << "\n";
	storeTargetOptr->printAddr(); std::cout << "\n";
	std::cout << "---------------------" << "\n";
	std::cout << "< check 4 " << "\n";

	omemcpy( storeTargetOptr.get() , exportedFlagmentHeader.get() , flagmentHeader->headerLength());

	omemcpy( (*storeTargetOptr + flagmentHeader->headerLength()).get() , qctx->value().get() , qctx->valueLength() );

	std::cout << "< check 5 " << "\n";

	return storeTargetOptr;
}





size_t ValueStoreManager::get( std::shared_ptr<optr> targetOptr ,std::shared_ptr<unsigned char> *ret )
{
	std::cout << "ValueStoreManager::get() called" << "\n";
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>(
		_dataOverlayMemoryManager->dataCacheTable()
	 );
	std::cout << "FlagmentHeader Initialized" << "\n";

	_dataOverlayMemoryManager->wrap( targetOptr ); // キャッシュテーブルを事前にセットしなければならない

	std::cout << "## check 1 " << "\n";
	
	flagmentHeader->importBinary( targetOptr );

	std::cout << "## check 2 " << "\n";
	
	// 本来はここで他フラグメントが存在する場合は,それに合わせて拡張する

	size_t valueLength = flagmentHeader->valueLength(); 


	std::cout << valueLength << "\n";
	flagmentHeader->_meta._prevFlagment->printAddr(); std::cout << "\n";
	flagmentHeader->_meta._nextFlagment->printAddr(); std::cout << "\n";

	*ret = std::shared_ptr<unsigned char>( new unsigned char[valueLength] );

	std::cout << "## check 3" << "\n";

	omemcpy( (*ret).get() , *targetOptr + (flagmentHeader->headerLength()) , flagmentHeader->valueLength() );

	std::cout << "## check 4" << "\n";

	return valueLength;
}





};
