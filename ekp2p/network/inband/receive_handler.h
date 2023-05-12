#ifndef D5A9D386_AFCE_4D35_8156_7129D3261250
#define D5A9D386_AFCE_4D35_8156_7129D3261250



#include <iostream>


namespace ekp2p{

struct ReceiveThreadArgs;


class ReceiveHandler{

public:

	// 最初に呼び出される
	void (* _bootHandler)( void *payload , unsigned int paylaodSize , void* free , ReceiveThreadArgs *receiveThreadArgs , struct sockaddr* peerAddr );
	void test();

	ReceiveHandler();
};




} // close ekp2p namespace


#endif // D5A9D386_AFCE_4D35_8156_7129D3261250

