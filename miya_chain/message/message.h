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

constexpr unsigned short MIYA_CHAIN_MSG_COMMAND_LENGTH = 12;




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
		char _command[12]; // プロトコルタイプ  with ASCII
		uint32_t _payloadLength;
	} _header;

	MiyaChainCommand _payload;

public:

	/* Getter */
	size_t payloadLength();
	const char *command();
	MiyaChainCommand payload();

	/* Setter */
	void payload( MiyaChainCommand targetPayload , const char *command );
	void payloadLength( size_t target );


	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  
	
	size_t exportRawCommand( const char* command , MiyaChainCommand commandBody , std::shared_ptr<unsigned char> *retRaw );
	static int commandIndex( const char* command );


} __attribute__((packed));




};













#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F



