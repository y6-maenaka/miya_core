#ifndef FCD6F110_7105_46A9_9076_7D10DA910106
#define FCD6F110_7105_46A9_9076_7D10DA910106


#include <arpa/inet.h>
#include <sys/socket.h>


namespace ekp2p{

class SocketManager;


class ClientNatManager
{

private:
	SocketManager *_socketManager;


public:
	ClientNatManager( SocketManager *seupedSocketManager = nullptr );

	bool natTraversal( struct sockaddr_in *globalAddr );
	
	bool validateSockaddrIn( sockaddr_in *targetAddr );
};



}; // close ekp2p namespace



#endif // FCD6F110_7105_46A9_9076_7D10DA910106

