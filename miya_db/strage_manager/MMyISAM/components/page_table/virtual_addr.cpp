#include "virtual_addr.h"



unsigned char *VirtualAddr::frame()
{
	return _virtualAddr;
}



unsigned char *VirtualAddr::offset()
{
	return _virtualAddr[3];
}
