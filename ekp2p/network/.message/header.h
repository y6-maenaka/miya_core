#ifndef BFFC5811_9233_4D52_805D_CD06BBC63927
#define BFFC5811_9233_4D52_805D_CD06BBC63927


#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>



namespace ekp2p{

//constexpr char *MSG_StartString = 0xf9beb4d9;
constexpr char MSG_StartString[4] = { static_cast<char>(0xf9), static_cast<char>(0xbe), static_cast<char>(0xb3), static_cast<char>(0xd9) };
constexpr char MSG_EndString[4]   = { static_cast<char>(0xff), static_cast<char>(0xff), static_cast<char>(0xff), static_cast<char>(0xff) };





struct MSGHeader{ // rename to EKP@PMSGHeader


	struct HeaderBody
	{
		char StartString[4];      // n(4)
		uint32_t PayloadSize;     // n(4)
		uint32_t KTagSize;        // n(4) // kTagCountに変えた方がいいかな KTagのサイズは固定長のため
		unsigned char Free[12];   // n(12)
		char EndString[4];       // n(4)
															
	} _body __attribute__(( aligned(4)) );

	MSGHeader();
	MSGHeader(  unsigned int payloadSize , unsigned int kTagSize, char* extra );

	void headerBody( struct MSGHeader::HeaderBody *headerBody ); // setter
	MSGHeader::HeaderBody *headerBody(); // getter	
	unsigned int headerBodySize();

	// success -> true(1)
	// bool GetOutHeaderElements();

	unsigned int rawMSGSize(); // getter
	unsigned char* free(); // getter

	void payloadSize( unsigned int payloadSize ); // setter
	unsigned int payloadSize(); // getter
													
	void kTagSize( unsigned int kTagSize ); // setter
	unsigned int kTagSize();  // getter

	char* startString(); // getter
};



} // close ekp2p namespace

#endif // BFFC5811_9233_4D52_805D_CD06BBC63927
