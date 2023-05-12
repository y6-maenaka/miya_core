#ifndef C896160C_7086_467D_BCC0_A206DAD1DC7F
#define C896160C_7086_467D_BCC0_A206DAD1DC7F

#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <errno.h>


namespace ekp2p{

class SocketManager{

private:
	int _sock;
	int _port;

public:
		// SocketManager();

	int create();
	int bind( int port );

	int sock(); // getter
	int port();

	int send( unsigned char* senfBuff, unsigned int senfBuffSize );

	/* send() によって振り分けられる */
	bool sendUDP();
	bool sendTCP();

};



}


#endif // C896160C_7086_467D_BCC0_A206DAD1DC7F
