#ifndef EEB5F4CC_6263_415A_9D65_56F14AD28224
#define EEB5F4CC_6263_415A_9D65_56F14AD28224


#include "./k_node.h"

#include <unistd.h>


namespace ekp2p
{

struct ConnectionControllerBody
{
	unsigned char _reserve[20];
	KNodeAddr _nodeAddr;

};







}; // close ekp2p namespace



#endif // EEB5F4CC_6263_415A_9D65_56F14AD28224

 


