#ifndef FCD6F110_7105_46A9_9076_7D10DA910106
#define FCD6F110_7105_46A9_9076_7D10DA910106


#include <arpa/inet.h>
#include <sys/socket.h>

#include <memory>
#include <thread>
#include <string>
#include <fstream>


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;


namespace ekp2p{

class SocketManager;
struct KNodeAddr;

constexpr unsigned short DEFAULT_GLOBAL_ADDR_INQUIRE_COUNT = 5;


class ClientNatManager
{

private:
	SocketManager *_socketManager;


public:

	std::shared_ptr<KNodeAddr> natTraversal( std::string stunServerAddrListPath ,std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toBrokerSBC );
	// bool natTraversal( struct sockaddr_in *globalAddr );

	bool validateSockaddrIn( sockaddr_in *targetAddr );
};



}; // close ekp2p namespace



#endif // FCD6F110_7105_46A9_9076_7D10DA910106
