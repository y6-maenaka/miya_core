#ifndef CC4E179A_0DF9_49F3_80E3_6F7CAA899C34
#define CC4E179A_0DF9_49F3_80E3_6F7CAA899C34




namespace ekp2p{

class SocketManager;
struct KTag;




class NodeOutband{

public:

	int send( unsigned char* payload, unsigned int payloadSize , KTag* kTag, SocketManager *socketManager );
	int send( unsigned char* msg , unsigned int msgSize , SocketManager *socketManager );

	int sendRaw();
};



};


#endif // CC4E179A_0DF9_49F3_80E3_6F7CAA899C34


