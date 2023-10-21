#ifndef DA8701DE_581B_4CD9_91FE_0936C87886E9
#define DA8701DE_581B_4CD9_91FE_0936C87886E9


#include <vector>

#include <arpa/inet.h>

// #include "openssl/crypto.h" // for use OPENSSL_free



namespace ekp2p{


struct KAddr;
class Node;
class SocketManager;


using KAddrVector = std::vector< KAddr* >;











struct KTag{

	struct KTagMeta
	{
		uint16_t Protocol;
		uint16_t KAddrCnt;
		u_char Free[12];	

		/* methods */
		bool validate(); unsigned short kAddrCnt();
		KTagMeta();

	} _KTagMeta __attribute__((__packed__));

	std::vector<KAddr*> _kAddrList;	
	
	// methods
	unsigned int exportRaw( unsigned char** ret );
	unsigned int exportRawSize();

	void importRaw( void* rawKTag , unsigned int kTagSize );

	KTag();
	KTag( KAddr *targetKAddr ); // import from kaddr
	KTag( void* rawKTag, unsigned int kTagSize );  // set kTag elements from raw k_tag

	unsigned char *generatePINGMSG();

	void protocol( unsigned short protocol ); // setter
	unsigned short protocol(); // getter 

	void addKAddr( KAddr *kAddr ); // setter
	void addKAddr( Node *node ); // setter
	void addKAddrBatch( std::vector< Node*> *nodeVector );

	std::vector< KAddr* > *kAddrVector();
};







struct KAddr
{
	// struct in_addr IP; // u_char[4]どっちがいい？
	// u_char _IPv4[4];
	
	struct inAddr{
		uint32_t _ipv4;
		uint16_t _port;
	} _inAddr __attribute__((__packed__));

	unsigned char _nodeID[20];
	// おそらくsockaddr_inで持ったほうがいい -> port(), ipv4()メソッドを導入したほうが良い
	
	KAddr();
	KAddr( sockaddr_in *addr );
	Node* toNode( SocketManager *socketManager = nullptr );	// socketManagerを登録していないとこのノードに対してデータを送信することができない
	unsigned char *nodeID(); // getter
	void nodeID( unsigned char *nodeID );

}__attribute__((__packed__));








}; // close ekp2p namespace


#endif // DA8701DE_581B_4CD9_91FE_0936C87886E9
