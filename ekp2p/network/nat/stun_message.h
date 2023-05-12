#ifndef C775E7FE_C209_4E01_B6DF_49E4442C88EC
#define C775E7FE_C209_4E01_B6DF_49E4442C88EC


#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>



namespace ekp2p{

constexpr uint16_t BINDING_REQUEST = 0x0001;
constexpr uint16_t BINDING_RESPONSE = 0x0010;

constexpr uint16_t MATCHING_REQUEST = 0x0011;
constexpr uint16_t MATCHING_RESPONSE = 0x0100;


struct RequesterAddr{
	u_char _ipv4[4];
	uint16_t _port;
} __attribute__((__packed__));



// サーバ用
/*
struct StunRequest{
	
	const int16_t _message_type = htons( BINDING_REQUEST );
	const int16_t _message_length = htons( 0x0000 );

} __attribute__((__packed__));
*/



struct StunResponse{
	
	uint16_t message_type;
	uint16_t message_size;
	struct RequesterAddr requesterAddr;

	unsigned int messageType(){ return ntohl(message_type); };

	void setAddr( struct sockaddr *peerAddr )
	{
		memcpy(  requesterAddr._ipv4 , &(((struct sockaddr_in *)peerAddr)->sin_addr) , 4 );
		requesterAddr._port = ((struct sockaddr_in *)peerAddr)->sin_port;

	};

} __attribute__ ((__packed__));








}; // close ekp2p namespace








#endif // C775E7FE_C209_4E01_B6DF_49E4442C88EC
