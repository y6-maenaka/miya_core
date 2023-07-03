#include "overlay_ptr.h"

#include "./overlay_ptr_resolver.h"



namespace miya_db{



OverlayPtr::OverlayPtr( unsigned char otpr )
{
	_optr	 = otpr;
}



const unsigned char OverlayPtr::optr()
{
	return _optr;
};


unsigned short OverlayPtr::frame()
{
	unsigned short ret = 0;
	ret = static_cast<unsigned short>(_optr & 0x18);
	return ret;
};




unsigned short OverlayPtr::offset()
{
	unsigned short ret = 0;
	ret = static_cast<unsigned short>(_optr & 0x03);
	return ret;
};



unsigned char OverlayPtr::value()
{
	return *(static_cast<unsigned char *>(_optResolver->resolve(this)));
}



void OverlayPtr::value( unsigned char target )
{
	*(static_cast<unsigned char *>(_optResolver->resolve(this))) = target;
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
	_optr = _from->optr();
	return this;
}




};



