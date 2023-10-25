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


	
std::shared_ptr<KNodeAddr> KNode::kNodeAddr()
{
	return _nodeAddr;
}



void KNode::kNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr )
{
	this->_nodeAddr = nodeAddr; // なぜかエラーが発生する
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


uint32_t KNodeAddr::ipv4()
{
	return _IPv4Addr;
}



uint16_t KNodeAddr::port()
{
	return _Port;
}





void KNodeAddr::setNodeID()
{
	std::shared_ptr<unsigned char> hashFrom = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_IPv4Addr)+sizeof(_Port)] );
	memcpy( hashFrom.get() , &_IPv4Addr , sizeof(_IPv4Addr) );
	memcpy( hashFrom.get() + sizeof(_IPv4Addr) , &_Port , sizeof(_Port) );


	std::shared_ptr<unsigned char> md; 
	hash::SHAHash( hashFrom, sizeof(_IPv4Addr) + sizeof(_Port), &md , "sha1" );
	memcpy( _ID , md.get() , 20 );

}



size_t KNodeAddr::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	size_t retRawLength = sizeof(struct KNodeAddr);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );
	memcpy( (*retRaw).get(), this , retRawLength );

	return retRawLength;
}


void KNodeAddr::printInfo()
{
	std::shared_ptr<struct sockaddr_in> addr = this->sockaddr_in();
	std::cout << "| IPv4 :: " << inet_ntoa(addr->sin_addr) << "\n";
	std::cout << "| port :: " << ntohs(addr->sin_port) << "\n";

	std::cout << "| NodeID :: ";
	for(int i=0; i<sizeof(_ID); i++ ){
		printf("%02X", _ID[i] );
	} std::cout << "\n";
}







KNode::KNode( std::shared_ptr<SocketManager> target )
{
	sockaddr_in ipv4Addr = target->sockaddr_in();
	// std::make_shared<KNodeAddr>( &ipv4Addr );
	_nodeAddr = std::shared_ptr<KNodeAddr>( new KNodeAddr(&ipv4Addr) );
}



KNode::KNode( std::shared_ptr<KNodeAddr> nodeAddr )
{
	_nodeAddr = nodeAddr;
}







void KHostNode::printInfo()
{
	std::cout << "--- [ K Client Node ] --------------------------------" << "\n";
	_nodeAddr->printInfo();
	std::cout << "------------------------------------------------------" << "\n";
}


};




