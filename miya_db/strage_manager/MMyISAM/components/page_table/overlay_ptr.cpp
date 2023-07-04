#include "overlay_ptr.h"

#include "./overlay_ptr_resolver.h"
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



OverlayPtr* OverlayPtr::operator =( OverlayPtr *_from )
{
	 // _optr = _from->optr();
	// return this;
}




};



