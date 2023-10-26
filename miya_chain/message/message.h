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

#include "./command/inv/inv.h"
#include "./command/block.h"
#include "./command/getblocks.h"
#include "./command/getdata.h"
#include "./command/getheaders.h"
#include "./command/headers.h"
#include "./command/mempool.h"
#include "./command/block.h"
#include "./command/notfound.h"

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


using MiyaChainCommand = std::variant<
								MiyaChainMSG_INV,
								MiyaChainMSG_BLOCK,
								MiyaChainMSG_GETBLOCKS,
								MiyaChainMSG_GETDATA,
								MiyaChainMSG_GETHEADERS,
								MiyaChainMSG_HEADERS,
								MiyaChainMSG_MEMPOOL,
								MiyaChainMSG_BLOCK,
								MiyaChainMSG_NOTFOUND
							>;



struct MiyaChainMessage
{
// private:
	struct 
	{
		unsigned char _token[4];
		unsigned char _command[12]; // プロトコルタイプ  with ASCII
		uint32_t _payloadLength;
	} _header;

	MiyaChainCommand _payload;

public:

	/* Getter */
	size_t payloadLength();
	const unsigned char *command();

	/* Setter */
	void payload( MiyaChainCommand targetPayload , unsigned char *command );

	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


} __attribute__((packed));




};













#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F



