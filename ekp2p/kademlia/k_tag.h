#ifndef DA8701DE_581B_4CD9_91FE_0936C87886E9
#define DA8701DE_581B_4CD9_91FE_0936C87886E9


#include <vector>

#include <arpa/inet.h>



namespace ekp2p{


struct KAddr;
class Node;
class SocketManager;






struct KTag{

	struct KTagMeta{

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

	KTag();
	KTag( void* rawKTag, unsigned int kTagSize );  // set kTag elements from raw k_tag

	unsigned char *generatePINGMSG();

	void protocol( unsigned short protocol ); // setter
	unsigned short protocol(); // getter 

	void addKAddr( KAddr *kAddr ); // setter
	void addKAddr( Node *node ); // setter
};





struct KAddr{

	// struct in_addr IP; // u_char[4]どっちがいい？
	u_char IPv4[4];
	uint16_t UDPPort;
	uint16_t TCPPort;
	
	KAddr();
	Node* toNode( SocketManager *socketManager = NULL );	// socketManagerを登録していないとこのノードに対してデータを送信することができない

	

}__attribute__((__packed__));








}; // close ekp2p namespace


#endif // DA8701DE_581B_4CD9_91FE_0936C87886E9
