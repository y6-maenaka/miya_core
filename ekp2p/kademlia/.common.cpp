#include "common.h"

#include "./k_tag.h"


namespace ekp2p{





unsigned int calcNodeID( sockaddr_in *targetAddr ,unsigned char **ret )
{

	KAddr tmpKAddr;
	tmpKAddr._inAddr._ipv4 = targetAddr->sin_addr.s_addr;
	tmpKAddr._inAddr._port = targetAddr->sin_port;

	return calcNodeID( &tmpKAddr , ret );
}





unsigned int calcNodeID( KAddr *targetKAddr, unsigned char **ret )
{

	unsigned int joinedAddrSize  = sizeof(KAddr::inAddr::_ipv4) + sizeof(KAddr::inAddr::_port) ;
	unsigned char* joinedAddr = new unsigned char[ joinedAddrSize ];
	memcpy( joinedAddr , &(targetKAddr->_inAddr._ipv4) , sizeof(KAddr::inAddr::_ipv4) );
	memcpy( joinedAddr + sizeof(KAddr::inAddr::_ipv4) , &(targetKAddr->_inAddr._port) , sizeof(KAddr::inAddr::_port) );

	*ret = hash::SHAHash( joinedAddr , joinedAddrSize , "SHA1" );

	return 20;
}













}// close ekp2p namespace
