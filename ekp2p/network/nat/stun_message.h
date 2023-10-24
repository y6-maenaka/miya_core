#ifndef C775E7FE_C209_4E01_B6DF_49E4442C88EC
#define C775E7FE_C209_4E01_B6DF_49E4442C88EC


#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

#include <memory>



namespace ekp2p{

constexpr uint16_t BINDING_REQUEST = 0x0001;
constexpr uint16_t BINDING_RESPONSE = 0x0010;

constexpr uint16_t MATCHING_REQUEST = 0x0011;
constexpr uint16_t MATCHING_RESPONSE = 0x0100;


struct RequesterAddr
{
	//u_char _ipv4[4];
	uint32_t _ipv4;
	uint16_t _port;

	void sockaddr_in( struct sockaddr_in from );
	struct sockaddr_in toSockaddr_in();
} __attribute__((__packed__));








struct StunRequest{

	const int16_t _message_type = htons( BINDING_REQUEST );
	const int16_t _message_length = htons( 0x0000 );

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
} __attribute__((__packed__));







struct StunResponse
{

private:
	uint16_t _message_type;
	uint16_t _message_size;

	struct RequesterAddr _requesterAddr;

public:
	unsigned int messageType(){ return ntohs(_message_type); };
	unsigned int message_size(){ return htons(_message_size); };

	void sockaddr_in( sockaddr_in from ); // setter

	unsigned int exportRaw( unsigned char **ret );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

	int importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
	unsigned int importRaw( unsigned char *from , unsigned int fromSize );

	struct sockaddr_in toSockaddr_in();


} __attribute__ ((__packed__));








}; // close ekp2p namespace








#endif // C775E7FE_C209_4E01_B6DF_49E4442C88EC
