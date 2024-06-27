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
#include <iomanip>

#include <json.hpp>
#include <node_gateway/services.hpp>
#include "boost/variant.hpp"

#include <node_gateway/message/command/inv/inv.hpp>
#include <node_gateway/message/command/block.hpp>
#include <node_gateway/message/command/getblocks.hpp>
#include <node_gateway/message/command/getdata.hpp>
#include <node_gateway/message/command/getheaders.hpp>
#include <node_gateway/message/command/headers.hpp>
#include <node_gateway/message/command/mempool.hpp>
#include <node_gateway/message/command/mempool.hpp>
#include <node_gateway/message/command/notfound.hpp>


using json = nlohmann::json;


namespace chain
{


constexpr unsigned short chain_PROTOCOL_BLOCK_HEADER = 1; // 固定長
constexpr unsigned short chain_PROTOCOL_BLOCK_DATA = 2; // 可変長
constexpr unsigned short chain_PROTOCOL_TX = 3; // 可変長 tx区切りあり

constexpr unsigned short chain_MSG_COMMAND_LENGTH = 12;


struct MiyaCoreMSG_NONE{};


struct miya_core_command : public std::enable_shared_from_this<struct miya_core_command>
{
public:
  using ref = std::shared_ptr<struct miya_core_command>;

  using command = std::variant<
	  MiyaCoreMSG_INV
	  , MiyaCoreMSG_GETBLOCKS
	  , MiyaCoreMSG_BLOCK
	  , MiyaCoreMSG_GETDATA
	  , MiyaCoreMSG_GETHEADERS
	  , MiyaCoreMSG_HEADERS
	  , MiyaCoreMSG_MEMPOOL
	  , MiyaCoreMSG_NOTFOUND

	  , MiyaCoreMSG_NONE
	>;
	
  enum command_type
  {
	  INV
	, BLOCK
	, GETBLOCKS
	, GETDATA
	, GETHEADERS
	, HEADERS
	, MEMPOOL
	, NOTFOUND

	, NONE
  };
 
private:
  command _command;
  command_type _command_t;
  bool _is_valid;

public:
  template < typename CommandType, typename... Args > miya_core_command( Args&&... args );
  template < typename T > miya_core_command( const T c );
  command_type get_command_type() const;

  template < typename T > T& get_command();  // variantの中身を返却する
  ref to_ref(); // return self shared_ptr

  std::vector<std::uint8_t> export_to_binary() const;
  bool import_from_binary( std::vector<std::uint8_t> from_v );
  bool is_valid() const;
  template <typename T > void set_command( const T c );

  template< typename T >
  std::optional<T> get() {
	if( auto ret = std::get_if<T>(_command); ret != nullptr ) return std::optional<T>(*ret);
	return std::nullopt;
  }
};

struct miya_core_message
{
  using ref = std::shared_ptr<miya_core_message>;

private:
  struct header
  {
	unsigned char _token[4];
	char _command[12];
	std::uint32_t _command_length;
  } _header __attribute__((packed));

  miya_core_command _command;
  bool _is_valid;

public:
  miya_core_message();
  miya_core_message( const json &from );
  miya_core_command &get_command();
  miya_core_command::ref get_command_ref();

  template <typename T> T& get_command();

  core::MIYA_SERVICES get_services() const;
  miya_core_command::command_type get_command_type() const;

  std::vector<std::uint8_t> export_to_binary() const;
  bool import_from_binary( std::vector<std::uint8_t> from );
  bool import_from_binary( const json &from );
};


template < typename CommandType, typename... Args > miya_core_command::miya_core_command( Args&&... args )  :
  _command( CommandType(std::forward<Args>(args)...) )
{
  return; 
} 

template < typename T > T& miya_core_command::get_command()
{
  return std::get<T>(_command);
}

template <typename T> T& miya_core_message::get_command()
{
  return _command.get_command<T>();
}


/* struct MiyaChainMessage
{
// private:
  struct header
  {
	  unsigned char _token[4];
	  char _command[12]; // プロトコルタイプ  with ASCII
	  std::uint32_t _payloadLength;
  } _header __attribute__((packed)); // 20 [bytse]

  miya_core_command<T> _command;

  bool _is_valid;
public:
  bool is_valid() const;

  MiyaChainMessage();

  // Getter
  size_t payloadLength();
  const char *command();
  MiyaCoreCommand payload();

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

  MiyaChainMessage::command_type commandIndex();
  static MiyaChainMessage::command_type commandIndex( const char* command );

  // Setter
  void payload( MiyaCoreCommand targetPayload , const char *command );
  void payloadLength( size_t target );

  bool import_from_binary( std::vector<unsigned char> from_v );
  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  std::vector<unsigned char> export_to_bianry() const;

  size_t exportRawCommand( const char* command , MiyaCoreCommand commandBody , std::shared_ptr<unsigned char> *retRaw ); // 各commandを書き出す

  static std::shared_ptr<MiyaChainMessage> decode( const json from_j ); // jsonからのデコード
  static json encode( const MiyaChainMessage &from ); // jsonにエンコード

} __attribute__((packed));
*/
// MiyaChainMessage decode_from_json( json from );


};


#endif // C5989909_FBC9_41AE_B4AC_2941FA1C329F
