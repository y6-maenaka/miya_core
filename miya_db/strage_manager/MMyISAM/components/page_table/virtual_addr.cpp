#include "virtual_addr.h"



namespace miya_db{


unsigned char *VirtualAddr::frame()
{
	return _virtualAddr;
}



unsigned char *VirtualAddr::offset()
{
	return &_virtualAddr[3];
}


}; // close miya_db namesapce
