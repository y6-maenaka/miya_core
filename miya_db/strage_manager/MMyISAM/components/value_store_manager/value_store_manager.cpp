#include "value_store_manager.h"


#include "../page_table/overlay_ptr.h"
#include "../page_table/optr_utils.h"
#include "../page_table/overlay_memory_manager.h"
#include "../page_table/cache_manager/cache_table.h"

#include "../../../../miya_db/query_context/query_context.h"

#include "../../safe_mode_manager/safe_btree.h"
#include "../../safe_mode_Manager/safe_o_node.h"

namespace miya_db
{





ValueFragmentHeader::ValueFragmentHeader( std::shared_ptr<CacheTable> cacheTable )
{
	//std::time_t timestamp = std::time(nullptr);
	uint64_t timestamp = static_cast<uint64_t>( std::time(nullptr) );
	_meta._timestamp = timestamp;

	std::shared_ptr<unsigned char> addrZero = std::shared_ptr<unsigned char>( new unsigned char[VALUE_FLAGMENT_ADDR_SIZE] );
	memset( addrZero.get() , 0x00 , VALUE_FLAGMENT_ADDR_SIZE );

	_meta._prevFlagment = std::shared_ptr<optr>( new optr(addrZero.get(), cacheTable ) );

	_meta._nextFlagment = std::shared_ptr<optr>( new optr(addrZero.get(), cacheTable ) );

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
	*ret = std::shared_ptr<unsigned char>( new unsigned char[DEFAULT_VALUE_FLAGMENT_HEADER_SIZE] );
	_meta._headerLength = DEFAULT_VALUE_FLAGMENT_HEADER_SIZE;


	std::cout << "------------------------------------------------" << "\n";
	std::cout << sizeof(_meta._headerLength) << " HeaderLength :: " << _meta._headerLength << "\n";
	std::cout << 5 << " PrevFlagment :: "; _meta._prevFlagment->printAddr(); std::cout << "\n";
	std::cout << 5 << " NextFlagment :: "; _meta._nextFlagment->printAddr(); std::cout << "\n";
	std::cout << sizeof(_meta._valueLength) << " ValueLength :: " << _meta._valueLength << "\n";
	std::cout << sizeof(_meta._timestamp) << " TiemStamp :: " << _meta._timestamp << "\n";
	std::cout << "------------------------------------------------" << "\n";

	int formatPtr = 0;
	memcpy( (*ret).get() , &(_meta._headerLength)  , sizeof(_meta._headerLength) ); formatPtr += sizeof(_meta._headerLength);

	omemcpy( (*ret).get() + formatPtr , _meta._prevFlagment.get() , VALUE_FLAGMENT_ADDR_SIZE ); formatPtr += VALUE_FLAGMENT_ADDR_SIZE;

	omemcpy( (*ret).get() + formatPtr , _meta._nextFlagment.get() , VALUE_FLAGMENT_ADDR_SIZE ); formatPtr += VALUE_FLAGMENT_ADDR_SIZE;

	memcpy( (*ret).get() + formatPtr , &(_meta._valueLength) , sizeof(_meta._valueLength) ); formatPtr += sizeof(_meta._valueLength);

	memcpy( (*ret).get() + formatPtr , &(_meta._timestamp) , sizeof(_meta._timestamp) ); formatPtr += sizeof(_meta._timestamp);

	return formatPtr;
}



















/*
ValueStoreManager::ValueStoreManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_dataOverlayMemoryManager = oMemoryManager;
}
*/


ValueStoreManager::ValueStoreManager( std::string valueFilePath )
{
	_dataOverlayMemoryManager = std::make_shared<OverlayMemoryManager>( valueFilePath );
	printf("ValueStoreManager Initialized with %p\n", _dataOverlayMemoryManager.get() );
}



std::shared_ptr<optr> ValueStoreManager::add( std::shared_ptr<unsigned char> data , size_t dataLength )
{
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>(
		_dataOverlayMemoryManager->dataCacheTable()
	);

	flagmentHeader->valueLength( dataLength );

	std::shared_ptr<unsigned char> exportedFlagmentHeader;
	size_t exportedFlagmentHeaderLength = flagmentHeader->exportBianry( &exportedFlagmentHeader );

	std::shared_ptr<optr> storeTargetOptr;
	storeTargetOptr = _dataOverlayMemoryManager->allocate( exportedFlagmentHeaderLength + dataLength );
	omemcpy( storeTargetOptr.get() , exportedFlagmentHeader.get() , flagmentHeader->headerLength());

	omemcpy( (*storeTargetOptr + flagmentHeader->headerLength()).get() , data.get() , dataLength  );

	return storeTargetOptr;
}


// あとで,中身はもう一方のaddを呼ぶようにする
std::shared_ptr<optr> ValueStoreManager::add( std::shared_ptr<QueryContext> qctx )
{
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>(
		_dataOverlayMemoryManager->dataCacheTable()
	);

	flagmentHeader->valueLength( qctx->valueLength() );

	std::shared_ptr<unsigned char> exportedFlagmentHeader;
	size_t exportedFlagmentHeaderLength = flagmentHeader->exportBianry( &exportedFlagmentHeader );

	std::shared_ptr<optr> storeTargetOptr;
	storeTargetOptr = _dataOverlayMemoryManager->allocate( exportedFlagmentHeaderLength + qctx->valueLength() );
	omemcpy( storeTargetOptr.get() , exportedFlagmentHeader.get() , flagmentHeader->headerLength());

	omemcpy( (*storeTargetOptr + flagmentHeader->headerLength()).get() , qctx->value().get() , qctx->valueLength() );

	return storeTargetOptr;
}





size_t ValueStoreManager::get( std::shared_ptr<optr> targetOptr ,std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<ValueFragmentHeader> flagmentHeader = std::make_shared<ValueFragmentHeader>(
		_dataOverlayMemoryManager->dataCacheTable()
	 );
	_dataOverlayMemoryManager->wrap( targetOptr ); // キャッシュテーブルを事前にセットしなければならない

	flagmentHeader->importBinary( targetOptr );

	// 本来はここで他フラグメントが存在する場合は,それに合わせて拡張する

	size_t valueLength = flagmentHeader->valueLength();
	*ret = std::shared_ptr<unsigned char>( new unsigned char[valueLength] );
	omemcpy( (*ret).get() , *targetOptr + (flagmentHeader->headerLength()) , flagmentHeader->valueLength() );

	return valueLength;
}



void ValueStoreManager::mergeDataOptr( std::shared_ptr<ONodeConversionTable> conversionTable ,ValueStoreManager* safeValueStoreManager )
{
	std::shared_ptr<unsigned char> data; size_t dataLength;
	for( auto itr : conversionTable->entryMap() )
	{
		// Entryに存在する要素のSafeONodeを作成する為第2引数のOMemoryManagerは必ずSafeOMemoryManagerになる
		std::shared_ptr<SafeONode> targetSafeONode = std::make_shared<SafeONode>( conversionTable , conversionTable->safeOMemoryManager() , std::make_shared<optr>(itr.first) ); // エントリに存在するSafeONodeの呼び出し
		for( int i=0; i< targetSafeONode->citemSet()->dataOptrCount() ; i++ )
		{
			if( itr.second._dataOptrLocation[i] == DATA_OPTR_LOCATED_AT_SAFE )
			{
					DataOptrEx dataOptr = targetSafeONode->citemSet()->dataOptr(i);
					dataLength = safeValueStoreManager->get( dataOptr.first , &data );
					std::shared_ptr<optr> copyedOptr = this->add( data , dataLength );

					targetSafeONode->itemSet()->dataOptr( i , std::make_pair( copyedOptr , DATA_OPTR_LOCATED_AT_NORMAL ));
					// itr.second._dataOptrLocation[i] = DATA_OPTR_LOCATED_AT_NORMAL; // 明示的に変更説ともdataOptr呼び出し時に変更される
			}
		}
		conversionTable->update( std::make_shared<optr>(itr.first) , std::make_shared<MappingContext>(itr.second) );
	}

	_dataOverlayMemoryManager->syncDBState(); // データベース内部状態の書き込み/保存
}



void ValueStoreManager::clear()
{
	_dataOverlayMemoryManager->clear();
}


const std::shared_ptr<OverlayMemoryManager> ValueStoreManager::overlayMemoryManager()
{
	return _dataOverlayMemoryManager;
}


};
