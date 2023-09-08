#ifndef D8DA7110_5FEA_457B_9798_647BC74D0F2C
#define D8DA7110_5FEA_457B_9798_647BC74D0F2C

#include <vector>
#include <array>
#include <memory>

#include "../../kademlia/k_node.h"



namespace ekp2p
{





/*
	-----------------------------------------------
	|   ヘッダ長  |   ペイロード長  |  プロトコル |
	-----------------------------------------------
	| バージョン |  パディング兼予約領域          |
	-----------------------------------------------
	|             送信元ノードアドレス            | 
	-----------------------------------------------
	|                 中継ノード                  |
	-----------------------------------------------
	|                 中継ノード                  |
	-----------------------------------------------
	|                 中継ノード                  |
	-----------------------------------------------
	|                 中継ノード                  |
	-----------------------------------------------
	|                 中継ノード                  |
	-----------------------------------------------
	|                                             |
	|                                             |
	|                                             |
	|                 ペイロード                  |
	|                                             |
	|                                             |
	-----------------------------------------------
*/




class KNodeAddr;



struct EKP2PMessageHeader
{
	struct __attribute__((packed))
 	{
		unsigned char _token[4]; // startString
		uint16_t _headerLength; // 2 [ bytes ]
		uint16_t _payloadLength; // 2 [ bytes [
		uint8_t _protocol; // 1 [ bytes ]
		uint8_t _version; // 1 [ bytes ]
	} _meta;


	std::shared_ptr<KNodeAddr> _sourceNodeAddr = nullptr ;
	std::vector< std::shared_ptr<KNodeAddr> > _relayKNodeAddrVector;


EKP2PMessageHeader();

/* Setter */
void protocol( unsigned short type ){ _meta._protocol = static_cast<uint8_t>(type); };
void payloadLength( unsigned short length ){ _meta._payloadLength = static_cast<uint16_t>(length); };
void sourceNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr );

/* Getter */
unsigned short headerLength();
unsigned short payloadLength();
unsigned short protocol();

bool validate();

unsigned int exportRaw( unsigned char** ret );
void importRaw( void *rawHeader , unsigned int rawHeaderSize );
};





}; // close ekp2p namespace


#endif // D8DA7110_5FEA_457B_9798_647BC74D0F2C



