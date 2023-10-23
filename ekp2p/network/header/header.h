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




struct KNodeAddr;



struct EKP2PMessageHeader
{
//private:
	struct Meta 
 	{
		unsigned char _token[4]; // startString
		uint16_t _headerLength ; // 2 [ bytes ] // 書き出しの時に値が挿入されるので基本的に気にしなくて良い
		uint16_t _payloadLength; // 2 [ bytes [
		uint16_t	 _rpcType; // 1 [bytes] ※ kademliaレベルでのptorocol
		uint16_t _protocol; // 1 [ bytes ] ※ 下層モジュールへのプロトコル
		uint8_t _version; // 1 [ bytes ]
		
		uint16_t	_relayKNodeAddrCount = 0;
		unsigned char reserved[5];

		size_t headerLength(){ return static_cast<size_t>(ntohs(_headerLength)); };
		size_t payloadLength(){ return static_cast<size_t>(ntohs(_payloadLength)); };
	} _meta __attribute__((packed));


	std::shared_ptr<KNodeAddr> _sourceKNodeAddr = nullptr ;
	std::vector< std::shared_ptr<KNodeAddr> > _relayKNodeAddrVector;

public:
	EKP2PMessageHeader();

	/* Setter */
	void rpcType( unsigned short type ){ _meta._rpcType = htons(type); };
	void protocol( unsigned short type ){ _meta._protocol = htons(type); };
	void payloadLength( unsigned short length ){ _meta._payloadLength = htons(length); };
	void sourceKNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr );
	void relayKNodeAddrVector( std::vector<std::shared_ptr<KNodeAddr>> targetVector );
		

	/* Getter */
	size_t headerLength();
	unsigned short payloadLength();
	unsigned short protocol();
	std::shared_ptr<KNodeAddr> sourceKNodeAddr();
	std::vector< std::shared_ptr<KNodeAddr> > relayKNodeAddrVector();
	unsigned char* token();
	unsigned short rpcType();


	bool validate();
	// unsigned int exportRaw( unsigned char** ret );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRawSequentially( std::shared_ptr<unsigned char> fromRaw );
	// void importRaw( void *rawHeader );
	void printRaw();
};





}; // close ekp2p namespace


#endif // D8DA7110_5FEA_457B_9798_647BC74D0F2C



