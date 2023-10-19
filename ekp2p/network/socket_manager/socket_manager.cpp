#include "socket_manager.h"

#include "../../kademlia/k_node.h"


namespace ekp2p{




SocketManager::SocketManager()
{
	memset( &_addr , 0x00 , sizeof(_addr) );
}




SocketManager::SocketManager( std::shared_ptr<KNodeAddr> fromKNodeAddr )
{
	memset( &_addr , 0x00 , sizeof(_addr) );

	// struct sockaddr_in nodeAddr; memset( &nodeAddr , 0x00 , sizeof(nodeAddr) );
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl( fromKNodeAddr->ipv4() );
	_addr.sin_port = htons( fromKNodeAddr->port() );

	// 一旦UDPで妥協
	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return;
	if( (::bind(_sock, (struct sockaddr *)&_addr, sizeof(_addr))) < 0 ) return;


	return;
}









int SocketManager::bind( int port )
{
	// struct sockaddr_in servAddr;

	memset( &_addr, 0x00 , sizeof(_addr) );
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl( INADDR_ANY ); //　引数で変更可にする
	_addr.sin_port = htons( port );

	if( (::bind(_sock, (struct sockaddr *)&_addr, sizeof(_addr))) < 0 ) return -1;

	_port = port;

	return port;
}




int SocketManager::sock()
{
	return _sock;
}



int SocketManager::port()
{
	return _port;
}



int SocketManager::send( unsigned char* sendBuff, unsigned int senfBuffSize )
{
	return ::send( _sock , sendBuff, senfBuffSize , 0 );
}




int SocketManager::send( std::shared_ptr<unsigned char> rawBuff, size_t rawBuffLength )
{
	return ::send( _sock , rawBuff.get() , rawBuffLength , 0 );
}






int SocketManager::setupUDPSock( unsigned short port )
{

	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return -1;

	if( bind( port ) < 0 ) return -1;

	return _sock;
}




void SocketManager::toBlockingSocket()
{
	int flag = fcntl( _sock , F_GETFL , 0 );
	fcntl( _sock , F_SETFL  , flag | O_NONBLOCK ); // NONBLOCKING_FLAGを追加
}


void SocketManager::toNonBlockingSocket()
{
	int flag = fcntl( _sock , F_GETFL , 0 );
	fcntl( _sock , F_SETFL , flag &= ~O_NONBLOCK ); // NON_BLOCKING属性を取り出す
	
}

/*
int SocketManager::sockType()
{
	int opt; socklen_t optLen = sizeof( opt );

	getsockopt( _sock , SOL_SOCKET , SO_TYPE , &opt , &optLen );
	if( opt == -1 ) return -1;

	return opt;
}
*/


struct sockaddr_in SocketManager::ipv4Addr()
{
	return _addr;
}



void SocketManager::sock( int bindedSock )
{
	_sock = bindedSock;
}


void SocketManager::addr( struct sockaddr_in target )
{
	_addr = target;
}





} // close ekp2p namespace 
