#ifndef F171E118_2D2F_4B8F_971F_E9DB7A90A1BC
#define F171E118_2D2F_4B8F_971F_E9DB7A90A1BC



#include <iostream>
#include <memory>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>



namespace ekp2p
{




class SocketManager;





struct KNodeAddr
{
// private:
	uint32_t _IPv4Addr;
	uint16_t _Port;
	unsigned char _ID[20];


public:
	KNodeAddr(){};
	KNodeAddr( struct sockaddr_in *addr );
	// KNodeAddr( unsigned long ipv4 , unsigned short port ); // アーキテクチャによってビット数が異なる可能性あり

	void setNodeID();


	std::shared_ptr<struct sockaddr_in> sockaddr_in();
	unsigned char* ID();

	uint32_t ipv4();
	uint16_t port();

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	void printInfo();

} __attribute__((packed));






class KNode
{

protected:
	std::shared_ptr<KNodeAddr> _nodeAddr;

public:
	// KNode();
	KNode( std::shared_ptr<SocketManager> target );
	KNode( std::shared_ptr<KNodeAddr> nodeAddr );

	std::shared_ptr<KNodeAddr> kNodeAddr();
	void kNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr );

};







class KClientNode : public KNode 
{

private:
	// SocketManager *_socketManager;
	// std::shared_ptr<SocketManager> _socketManager;

public:
	KClientNode( std::shared_ptr<KNodeAddr> nodeAddr ) : KNode( nodeAddr ){};
	

	// std::shared_ptr<SocketManager> socketManager();

};






class KHostNode : public KNode
{
private:
	void dummy();
public:
	KHostNode( std::shared_ptr<SocketManager> fromSockManager ) : KNode( fromSockManager ){} ;

	void printInfo();
};






}; // close ekp2p namespace






#endif // F171E118_2D2F_4B8F_971F_E9DB7A90A1BC

