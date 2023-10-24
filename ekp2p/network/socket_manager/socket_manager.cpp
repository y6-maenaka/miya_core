#include "socket_manager.h"

#include "../../kademlia/k_node.h"
#include "../../network/message/message.h"
#include "../../network/header/header.h"


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
	_addr.sin_addr.s_addr =  fromKNodeAddr->ipv4();
	_addr.sin_port = fromKNodeAddr->port();


	// 一旦UDPで妥協
	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return;
	// if( (::bind(_sock, (struct sockaddr *)&_addr, sizeof(_addr))) < 0 ) return;


	return;
}


SocketManager::SocketManager( struct sockaddr_in fromAddr )
{
	memset( &_addr , 0x00 , sizeof(_addr) );
	_addr = fromAddr;

	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return;

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
	return ::sendto( _sock , sendBuff, senfBuffSize , 0 , (struct sockaddr*)&_addr , sizeof(_addr) );
}





int SocketManager::send( std::shared_ptr<unsigned char> rawBuff, size_t rawBuffLength )
{
	return ::sendto( _sock , rawBuff.get() , rawBuffLength , 0 , (struct sockaddr*)&_addr , sizeof(_addr) );
}



int SocketManager::send( std::shared_ptr<EKP2PMessage> msg )
{
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSGLength = msg->exportRaw( &rawMSG );

	return ::sendto( _sock , rawMSG.get() , rawMSGLength , 0 , (struct sockaddr *)&_addr , sizeof(_addr) );
}



size_t SocketManager::receive( std::shared_ptr<unsigned char> *retRaw , struct sockaddr_in &fromAddr )
{
	unsigned int fromAddrLength = sizeof( fromAddr );
	memset( &fromAddr , 0x00 , sizeof(fromAddr) );
	struct EKP2PMessageHeader::Meta _headerMeta; // なぜかEKP2PMessageHeaderを宣言するとエラー落ちする
	recvfrom( _sock, &_headerMeta , sizeof(struct EKP2PMessageHeader::Meta), MSG_PEEK, (struct sockaddr *)&fromAddr, &fromAddrLength ); // セグメントの受信
	
	size_t rawMSGLength = _headerMeta.headerLength() + _headerMeta.payloadLength();

	std::cout << "++++++++++++++++++++++++++++++" << "\n";
	std::cout << "This is SocketManager::receive" << "\n";
	std::cout << "received from ip :: " << inet_ntoa(fromAddr.sin_addr) << "\n";
	std::cout << "received from port :: " << ntohs(fromAddr.sin_port) << "\n";
	std::cout << "++++++++++++++++++++++++++++++" << "\n";
	
	size_t retLength;
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[rawMSGLength] );
	retLength = recvfrom( _sock, (*retRaw).get() , rawMSGLength , 0, nullptr, 0 ); // セグメントの受信

	return retLength;
}



int SocketManager::setupUDPSock( unsigned short port )
{
	if ( (this->_sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )  return -1;

	if( this->bind( port ) < 0 ) return -1;

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
