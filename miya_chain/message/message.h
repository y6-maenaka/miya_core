#ifndef C5989909_FBC9_41AE_B4AC_2941FA1C329F
#define C5989909_FBC9_41AE_B4AC_2941FA1C329F


#include <memory>



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

	uint8_t _protocol; // プロトコルタイプ 
	uint8_t _direction; // リクエスト or レスポンス
	uint16_t _payloadLength;
	unsigned char _reserved[4];


	unsigned short protocol();
	bool isRequest();

} __attribute__((packed));





struct MiyaChainMessage
{
	std::shared_ptr<MiyaChainMessageHeader> _header;
	void *_payload;


	MiyaChainMessage();

	void payload( void *payload , unsigned short payloadLength );
	unsigned short protocol();

	unsigned int exportRaw( std::shared_ptr<unsigned char> ret );

	std::shared_ptr<BlockHeaderMessage> blockHeaderMSG();
	std::shared_ptr<BlockDataResponseMessage> blockDataResponseMSG();
};




};













#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F



