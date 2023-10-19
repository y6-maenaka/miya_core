#ifndef C5989909_FBC9_41AE_B4AC_2941FA1C329F
#define C5989909_FBC9_41AE_B4AC_2941FA1C329F


#include <memory>
#include <endian.h>



namespace miya_chain
{


struct BlockHeaderMessage;
struct BlockDataRequestMessage;
struct BlockDataResponseMessage;



constexpr unsigned short MIYA_CHAIN_PROTOCOL_BLOCK_HEADER = 1; // 固定長
constexpr unsigned short MIYA_CHAIN_PROTOCOL_BLOCK_DATA = 2; // 可変長
constexpr unsigned short MIYA_CHAIN_PROTOCOL_TX = 3; // 可変長 tx区切りあり





/*
 ----------------------------
 protocol | direction
 -----------------------------
 payloadLength | 
 ----------------------------
*/



struct MiyaChainMessageHeader
{
// private:
	uint8_t _protocol; // プロトコルタイプ 
	uint8_t _direction; // リクエスト or レスポンス
	uint16_t _payloadLength;
	unsigned char _reserved[4];

public:
	unsigned short protocol();
	bool isRequest();
	bool importRawSequentially( std::shared_ptr<unsigned char> fromRaw );
	size_t payloadLength();

} __attribute__((packed));





struct MiyaChainMessage
{
private:
	std::shared_ptr<MiyaChainMessageHeader> _header;
	// void *_payload;
	std::shared_ptr<unsigned char> _payload;

public:
	MiyaChainMessage();

	// void payload( void *payload , unsigned short payloadLength );
	unsigned short protocol();

	unsigned int exportRaw( std::shared_ptr<unsigned char> ret );

	std::shared_ptr<MiyaChainMessageHeader> header();

	void payload( std::shared_ptr<unsigned char> target );
	std::shared_ptr<unsigned char> payload();


};




};













#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F



