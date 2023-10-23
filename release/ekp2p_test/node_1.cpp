#include "node_1.h"

#include "../../ekp2p/network/message/message.h"
#include "../../ekp2p/network/header/header.h"

#include "../../ekp2p/network/socket_manager/socket_manager.h"
#include "../../ekp2p/daemon/routing_table_manager/__test.cpp"
#include "../../ekp2p/protocol.h"


int main()
{

	std::cout << "This is Node1" << "\n";

	std::shared_ptr<unsigned char> content = std::shared_ptr<unsigned char>( new unsigned char[10] ); 
	memcpy( content.get() , "HelloWorld" , 10 );

	struct sockaddr_in sourceAddr;
	sourceAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sourceAddr.sin_port = htons(8070);
	sourceAddr.sin_family = AF_INET;

	std::cout << "KADEMLIA_RPC_PING :: " << ekp2p::KADEMLIA_RPC_PING << "\n";
	ekp2p::EKP2P::sendDimmyEKP2PMSG( "127.0.0.1", 8080 , content , 10 , sourceAddr , ekp2p::KADEMLIA_RPC_PING , 1);


	std::shared_ptr<ekp2p::EKP2PMessage> msg;
	msg = ekp2p::EKP2P::receiveSingleEKP2PMSG( 8070 );
	msg->header()->printRaw();

	/*
	sleep(1);
	std::cout << "KADEMLIA_RPC_PING :: " << ekp2p::KADEMLIA_RPC_PING << "\n";
	ekp2p::EKP2P::sendDimmyEKP2PMSG( "127.0.0.1", 8080 , content , 10 , sourceAddr ,ekp2p::KADEMLIA_RPC_PING , 1 );
	*/

	//ekp2p::ping_wait_queue_unit_test();

	return 0;

	/*
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
	*/
}
