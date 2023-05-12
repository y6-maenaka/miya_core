#include "socket_manager.h"


namespace ekp2p{




int SocketManager::create()
{

	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return -1;

	return _sock;

}





int SocketManager::bind( int port )
{

	struct sockaddr_in servAddr;

	memset( &servAddr, 0x0 , sizeof(servAddr) );
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_ANY ); //　引数で変更可にする
	servAddr.sin_port = htons( port );

	if( (::bind(_sock, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0 ) return -1;

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





} // close ekp2p namespace 
