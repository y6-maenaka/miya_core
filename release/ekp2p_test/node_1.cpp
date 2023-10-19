#include "node_1.h"

#include "../../ekp2p/network/socket_manager/socket_manager.h"


int main()
{

	std::cout << "This is Node1" << "\n";


	int sock;
	sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = 8080;
	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr);
	bind( sock , (struct sockaddr *)&destAddr , sizeof(destAddr) );

	std::shared_ptr<ekp2p::SocketManager> socketManager = std::make_shared<ekp2p::SocketManager>();
	socketManager->sock( sock );
	socketManager->addr( destAddr );

	std::shared_ptr<unsigned char> data = std::shared_ptr<unsigned char>( new unsigned char[10] );
	memcpy( data.get() , "HelloWorld" , 10 );
	socketManager->send( data , 10 );
}
