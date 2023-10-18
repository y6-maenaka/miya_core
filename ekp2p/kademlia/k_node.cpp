#include "./k_node.h"

#include "../../shared_components/hash/sha_hash.h"

#include "../network/header/header.h"
#include "../network/socket_manager/socket_manager.h"

namespace ekp2p
{





KNodeAddr::KNodeAddr( struct sockaddr_in *addr )
{
	_IPv4Addr = addr->sin_addr.s_addr;
	_Port = addr->sin_port;
	setNodeID();

}





KNodeAddr::KNodeAddr( unsigned long ipv4 , unsigned short port )
{
	_IPv4Addr = ipv4;
	_Port = port;
	setNodeID();
}



	
std::shared_ptr<KNodeAddr> KNode::kNodeAddr()
{
	return _nodeAddr;
}



void KNode::kNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr )
{
	this->_nodeAddr = nodeAddr; // なぜかエラーが発生する
	//this->_nodeAddr->_IPv4Addr = nodeAddr->_IPv4Addr;
	//this->_nodeAddr->_Port = nodeAddr->_Port;

	_nodeAddr->setNodeID();
}




std::shared_ptr<struct sockaddr_in> KNodeAddr::sockaddr_in()
{
	struct sockaddr_in ret;

	ret.sin_family = AF_INET;
	ret.sin_addr.s_addr = _IPv4Addr;
	ret.sin_port = _Port;

	return std::make_shared<struct sockaddr_in>( ret );
}



unsigned char* KNodeAddr::ID()
{
	return _ID;
}


uint64_t KNodeAddr::ipv4()
{
	return _IPv4Addr;
}



uint16_t KNodeAddr::port()
{
	return _Port;
}






void KNodeAddr::setNodeID()
{
	unsigned char hashFrom[ sizeof(_IPv4Addr) + sizeof(_Port) ];
	unsigned char* ret;
	ret = hash::SHAHash( hashFrom , sizeof(hashFrom) , "sha1" );
	memcpy( _ID , ret , 20 );
	hash::FreeDigest( ret );	
}





KNode::KNode( std::shared_ptr<SocketManager> target )
{
	sockaddr_in ipv4Addr = target->ipv4Addr();
	// std::make_shared<KNodeAddr>( &ipv4Addr );
	_nodeAddr = std::shared_ptr<KNodeAddr>( new KNodeAddr(&ipv4Addr) );
	
}



KNode::KNode( std::shared_ptr<KNodeAddr> nodeAddr )
{
	_nodeAddr = nodeAddr;
}





std::shared_ptr<SocketManager> KClientNode::socketManager()
{
	return _socketManager;
}




};




