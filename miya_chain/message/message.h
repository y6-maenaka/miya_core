#ifndef C5989909_FBC9_41AE_B4AC_2941FA1C329F
#define C5989909_FBC9_41AE_B4AC_2941FA1C329F


#include <memory>
#ifdef __linux__
	#include <endian.h>
#endif
#ifdef __APPLE__
	#include <machine/endian.h>
#endif

#include <iostream>
#include <memory>
#include <variant>
#include <unordered_map>
#include <map>
#include <string>

#include "./command/inv/inv.h"
#include "./command/block.h"
#include "./command/getblocks.h"
#include "./command/getdata.h"
#include "./command/getheaders.h"
#include "./command/headers.h"
#include "./command/mempool.h"
#include "./command/notfound.h"

namespace miya_chain
{


struct BlockHeaderMessage;
struct BlockDataRequestMessage;
struct BlockDataResponseMessage;






constexpr unsigned short MIYA_CHAIN_PROTOCOL_BLOCK_HEADER = 1; // 固定長
constexpr unsigned short MIYA_CHAIN_PROTOCOL_BLOCK_DATA = 2; // 可変長
constexpr unsigned short MIYA_CHAIN_PROTOCOL_TX = 3; // 可変長 tx区切りあり

constexpr unsigned short MIYA_CHAIN_MSG_COMMAND_LENGTH = 12;




/*
 ----------------------------
 protocol | direction
 -----------------------------
 payloadLength |
 ----------------------------
*/


using MiyaChainCommand = std::variant<
								MiyaChainMSG_INV, // (0)
								MiyaChainMSG_BLOCK, // (1)
								MiyaChainMSG_GETBLOCKS, // (2)
								MiyaChainMSG_GETDATA, // (3)
								MiyaChainMSG_GETHEADERS, // (4)
								MiyaChainMSG_HEADERS, // (5)
								MiyaChainMSG_MEMPOOL, // (6)
								MiyaChainMSG_NOTFOUND // (7)
							>;



enum class MiyaChainCommandIndex
{
	MiyaChainMSG_INV, // (0)
	MiyaChainMSG_BLOCK, // (1)
	MiyaChainMSG_GETBLOCKS, // (2)
	MiyaChainMSG_GETDATA, // (3)
	MiyaChainMSG_GETHEADERS, // (4)
	MiyaChainMSG_HEADERS, // (5)
	MiyaChainMSG_MEMPOOL, // (6)
	MiyaChainMSG_NOTFOUND // (7)
};




struct MiyaChainMessage
{
// private:
	struct
	{
		unsigned char _token[4];
		char _command[12]; // プロトコルタイプ  with ASCII
		uint32_t _payloadLength;
	} _header; // 20 [bytse]

	MiyaChainCommand _payload;

public:
	MiyaChainMessage();

	/* Getter */
	size_t payloadLength();
	const char *command();
	unsigned short commandIndex();
	static int commandIndex( const char* command );
	MiyaChainCommand payload();

	/* Setter */
	void payload( MiyaChainCommand targetPayload , const char *command );
	void payloadLength( size_t target );

	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

	size_t exportRawCommand( const char* command , MiyaChainCommand commandBody , std::shared_ptr<unsigned char> *retRaw );

} __attribute__((packed));




};













#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F
