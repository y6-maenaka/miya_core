#include "overlay_ptr.h"

#include "./cache_manager/cache_table.h"



namespace miya_db{



OverlayPtr::OverlayPtr( unsigned char *optr )
{
	memcpy( _optr, optr , sizeof( _optr ));
}





void OverlayPtr::cacheTable( CacheTable *cacheTable )
{
	_cacheTable = cacheTable;
}


unsigned char *OverlayPtr::optr()
{
	return _optr;
};


void OverlayPtr::optr( unsigned long ulongOptr )
{
	_optr[0] = (ulongOptr >> 32) & 0xFF;
	_optr[1] = (ulongOptr >> 24) & 0xFF;
	_optr[2] = (ulongOptr >> 16) & 0xFF;
	_optr[3] = (ulongOptr >> 8) & 0xFF;
	_optr[4] = (ulongOptr >> 0) & 0xFF;
}



unsigned short OverlayPtr::frame()
{
	unsigned short ret = 0;
	
	ret += static_cast<unsigned short>(_optr[0]);
	ret += static_cast<unsigned short>(_optr[1]);
	ret += static_cast<unsigned short>(_optr[2]);

	return ret;	
	//ret = static_cast<unsigned short>(_optr & 0x18);
	//return ret;
};




unsigned short OverlayPtr::offset()
{
	unsigned short ret = 0;

	ret += static_cast<unsigned short>(_optr[3]);
	ret += static_cast<unsigned short>(_optr[4]);

	return ret;
};



unsigned char OverlayPtr::value()
{
	return *(static_cast<unsigned char *>(_cacheTable->convert(this)));
}



void OverlayPtr::value( unsigned char target )
{
	unsigned char *tmp;
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




std::unique_ptr<OverlayPtr> OverlayPtr::operator +( unsigned long addend )
{
	uint64_t ulongOptr = 0;

	unsigned short exponentialList[5] = {64, 32, 16, 8, 0};
	
	ulongOptr += static_cast<unsigned long>(_optr[0]) * pow(2, exponentialList[0]) ;
	ulongOptr += static_cast<unsigned long>(_optr[1]) * pow(2, exponentialList[1]) ;
	ulongOptr += static_cast<unsigned long>(_optr[2]) * pow(2, exponentialList[2]) ;
	ulongOptr += static_cast<unsigned long>(_optr[3]) * pow(2, exponentialList[3]) ;
	ulongOptr += static_cast<unsigned long>(_optr[4]) * pow(2, exponentialList[4]) ;

	ulongOptr += addend;
	std::cout << ulongOptr << "\n";
	
	std::unique_ptr<OverlayPtr> newOptr( new OverlayPtr );
	newOptr->cacheTable( _cacheTable );
	newOptr->optr( ulongOptr );

	for( int i=0; i<5; i++){
		printf("%02X- ", (newOptr->optr())[i]);
	}std::cout << "\n";


	return newOptr;
}




};



