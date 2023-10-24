#ifndef C896160C_7086_467D_BCC0_A206DAD1DC7F
#define C896160C_7086_467D_BCC0_A206DAD1DC7F

#include <iostream>
#include <memory>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>

#include <errno.h>


namespace ekp2p{



struct KNodeAddr;
struct EKP2PMessage;



class SocketManager{

private:
	int _sock;

	int _port;
	// int _type;
	struct sockaddr_in _addr;

public:
	SocketManager( std::shared_ptr<KNodeAddr> fromKNodeAddr );
	SocketManager( struct sockaddr_in fromAddr );
	SocketManager();

	// int create();
	int bind( int port );
	int listen();
	int accept();

	int sock(); // getter
	int port();

	int send( unsigned char* senfBuff, unsigned int senfBuffSize );
	int send( std::shared_ptr<unsigned char> rawBuff, size_t rawBuffLength );
	int send( std::shared_ptr<EKP2PMessage> msg );

	size_t receive( std::shared_ptr<unsigned char> *retRaw , struct sockaddr_in &fromAddr );
	/* send() によって振り分けられる */
	// bool sendUDP();
	// bool sendTCP();

	int setupUDPSock( unsigned short port );
	
	void toBlockingSocket();
	void toNonBlockingSocket();

	// int sockType();

	struct sockaddr_in ipv4Addr();

	/* テスト用 本番環境では極力使用しない*/
	void sock( int bindedSock );
	void addr( struct sockaddr_in target );
};






}


#endif // C896160C_7086_467D_BCC0_A206DAD1DC7F
