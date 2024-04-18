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
#include <optional>

#include <json.hpp>

// #include <json.hpp>

#include "./command/inv/inv.h"
#include "./command/block.h"
#include "./command/getblocks.h"
#include "./command/getdata.h"
// #include "./command/headers.h"
#include <node_gateway/message/command/headers.h>
#include "./command/getheaders.h"
#include "./command/mempool.h"
#include "./command/notfound.h"


using json = nlohmann::json;


namespace chain
{


struct BlockHeaderMessage;
struct BlockDataRequestMessage;
struct BlockDataResponseMessage;


constexpr unsigned short chain_PROTOCOL_BLOCK_HEADER = 1; // 固定長
constexpr unsigned short chain_PROTOCOL_BLOCK_DATA = 2; // 可変長
constexpr unsigned short chain_PROTOCOL_TX = 3; // 可変長 tx区切りあり

constexpr unsigned short chain_MSG_COMMAND_LENGTH = 12;


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
  
  bool _is_valid;
public:
  bool is_valid() const;

  MiyaChainMessage();

  /* Getter */
  size_t payloadLength();
  const char *command();
  unsigned short commandIndex();
  static int commandIndex( const char* command );
  MiyaChainCommand payload();

  enum command_type
  {
	MiyaCoreMSG_INV
	, MiyaCoreMSG_BLOCK
	, MiyaCoreMSG_GETBLOCKS
	, MiyaCoreMSG_GETDATA
	, MiyaCoreMSG_GETHEADERS
	, MiyaCoreMSG_HEADERS
	, MiyaCoreMSG_MEMPOOL
	, MiyaCoreMSG_NOTFOUND
  };
  command_type _command_t;
  command_type get_command_type();

  /* Setter */
  void payload( MiyaChainCommand targetPayload , const char *command );
  void payloadLength( size_t target );

  bool import_from_binary( std::vector<unsigned char> from_v );
  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  std::vector<unsigned char> export_to_bianry() const;

  size_t exportRawCommand( const char* command , MiyaChainCommand commandBody , std::shared_ptr<unsigned char> *retRaw );

  static std::shared_ptr<MiyaChainMessage> decode( const json from_j ); // jsonからのデコード
  static json encode( const MiyaChainMessage &from ); // jsonにエンコード

} __attribute__((packed));

// MiyaChainMessage decode_from_json( json from );


};


#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F
