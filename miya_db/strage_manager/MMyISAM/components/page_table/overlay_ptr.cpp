#include "overlay_ptr.h"

#include "./cache_manager/cache_table.h"
#include "./optr_utils.h"



namespace miya_db{



optr::optr( unsigned char *addr )
{
	memcpy( _addr, addr , sizeof( _addr ));
}




/*
void optr::cacheTable( CacheTable *cacheTable )
{
	_cacheTable = cacheTable;
}
*/

void optr::cacheTable( std::shared_ptr<CacheTable> cacheTable )
{
	_cacheTable = cacheTable;
}



optr::optr( unsigned char *addr , std::shared_ptr<CacheTable> cacheTable )
{
	memcpy( _addr, addr , sizeof( _addr ));
	_cacheTable = cacheTable;
}


unsigned char *optr::addr()
{
	return _addr;
};



std::shared_ptr<unsigned char> optr::caddr() const
{
	std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_addr)] );
	memcpy( ret.get(), _addr , sizeof(_addr) ); 
	return ret;
}



void optr::addr( unsigned long ulongOptr )
{
	_addr[0] = (ulongOptr >> 32) & 0xFF;
	_addr[1] = (ulongOptr >> 24) & 0xFF;
	_addr[2] = (ulongOptr >> 16) & 0xFF;
	_addr[3] = (ulongOptr >> 8) & 0xFF;
	_addr[4] = (ulongOptr >> 0) & 0xFF;
}



void optr::addr( optr* from )
{
	//omemcpy( _addr , from , sizeof(_addr) );
	memcpy( _addr , from->addr() , sizeof(_addr) );
}


void optr::addr( unsigned char* from )
{
	memcpy( _addr , from, 5 );
}


unsigned short optr::frame() const
{
	unsigned short ret = 0;
	
	unsigned short exponentialList[5] = {64, 32, 16, 8, 0}; // 変換用の累乗リスト																												//


	ret += static_cast<unsigned short>(_addr[0]) * pow( 2, exponentialList[2]);
	ret += static_cast<unsigned short>(_addr[1]) * pow( 2, exponentialList[3]);
	ret += static_cast<unsigned short>(_addr[2]) * pow( 2, exponentialList[4]);

	return ret;	
};




unsigned short optr::offset() const
{
	unsigned short ret = 0;


	unsigned short exponentialList[5] = {64, 32, 16, 8, 0}; // 変換用の累乗リスト																												//

	ret += static_cast<unsigned short>(_addr[3]) * pow(2,exponentialList[3]);
	ret += static_cast<unsigned short>(_addr[4]) * pow(2,exponentialList[4]);

	return ret;
};



unsigned char optr::value()
{
	return *(static_cast<unsigned char *>(_cacheTable->convert(this)));
}



void optr::value( unsigned char target )
{
	//*(static_cast<unsigned char *>(_cacheTable->convert(this))) = target;
	*(static_cast<unsigned char *>(_cacheTable->convert(this))) = target;	
	// メモリマップされたファイルのポインタ位置に値を格納
}

/*
unsigned char* OverlayPtr::operator []( size_t n )
{
	// OverlayPtr *newOptr = new OverlayPtr( _optr || static_cast<unsigned char>(n) );
	
				return *(static_cast<unsigned char*>(resolve(_optr || static_cast<unsigned char(n))));
};
*/




std::shared_ptr<optr> optr::operator +( unsigned long addend ) const
{
	uint64_t ulongOptr = 0;

	unsigned short exponentialList[5] = {64, 32, 16, 8, 0};
	
	ulongOptr += static_cast<unsigned long>(_addr[0]) * pow(2, exponentialList[0]) ;
	ulongOptr += static_cast<unsigned long>(_addr[1]) * pow(2, exponentialList[1]) ;
	ulongOptr += static_cast<unsigned long>(_addr[2]) * pow(2, exponentialList[2]) ;
	ulongOptr += static_cast<unsigned long>(_addr[3]) * pow(2, exponentialList[3]) ;
	ulongOptr += static_cast<unsigned long>(_addr[4]) * pow(2, exponentialList[4]) ;

	ulongOptr += addend;
	
	std::shared_ptr<optr> newOptr( new optr );
	newOptr->cacheTable( _cacheTable );
	newOptr->addr( ulongOptr );

	return newOptr;
}








std::unique_ptr<unsigned char> optr::mapToMemory( unsigned int size )
{
	//unsigned char* ret = new unsigned char[size];
	std::unique_ptr<unsigned char> ret = std::unique_ptr<unsigned char>( new unsigned char[size] );

	omemcpy( ret.get() , this ,static_cast<unsigned long>(size) );
	
	return ret;


	// exportarrayの実装
	// importの実装
}



void optr::printAddr() const
{
	for( int i=0; i<OPTR_ADDR_LENGTH; i++ )
		printf("%02X", _addr[i] );
}



void optr::printValueContinuously( unsigned int length ) const
{
	for( int i=0; i<length; i++ )
	{
		printf("%02X", (*this + i)->value() );
	}
}


};



