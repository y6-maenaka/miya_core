#include "overlay_ptr.h"



namespace miya_db{




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






};
