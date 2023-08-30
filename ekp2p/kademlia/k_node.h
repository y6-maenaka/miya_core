#ifndef F171E118_2D2F_4B8F_971F_E9DB7A90A1BC
#define F171E118_2D2F_4B8F_971F_E9DB7A90A1BC



#include <iostream>
#include <memory>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>



namespace ekp2p
{




class SocketManager;





struct KNodeAddr
{
	uint64_t _IPv4Addr;
	uint16_t _Port;
	unsigned char _ID[20];

	KNodeAddr(){};
	KNodeAddr( struct sockaddr_in *addr );
	KNodeAddr( unsigned long ipv4 , unsigned short port ); // アーキテクチャによってビット数が異なる可能性あり

	void setNodeID();

	std::shared_ptr<struct sockaddr_in> sockaddr_in();

} __attribute__((packed));






class KNode
{

private:
	std::shared_ptr<KNodeAddr> _nodeAddr;

public:
	KNode( std::shared_ptr<KNodeAddr> nodeAddr );

	std::shared_ptr<KNodeAddr> kNodeAddr();
	void kNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr );
};







class KClientNode : public KNode 
{

private:
	SocketManager *_socketManager;

public:
	KClientNode( std::shared_ptr<KNodeAddr> nodeAddr , SocketManager *socketManager ) : KNode( nodeAddr ){ _socketManager = socketManager; };
};






class KHostNode : public KNode
{
private:
	void dummy();
};






}; // close ekp2p namespace






#endif // F171E118_2D2F_4B8F_971F_E9DB7A90A1BC

