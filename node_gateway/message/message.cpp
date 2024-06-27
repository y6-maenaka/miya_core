#include "message.hpp"


namespace chain
{


miya_core_command::command_type miya_core_command::get_command_type() const
{
  return _command_t;
}

miya_core_command::ref miya_core_command::to_ref() 
{
  return shared_from_this();
}

std::vector<std::uint8_t> miya_core_message::export_to_binary() const
{
  if( !(_command.is_valid()) ) return std::vector<std::uint8_t>(); // if member of command is invalid

  std::vector<std::uint8_t> command_binary = _command.export_to_binary();
  if( command_binary.size() <= 0 ) return std::vector<std::uint8_t>(); // if command binary is invalid

  std::vector< std::uint8_t > header_binary; header_binary.resize( sizeof(_header) ); // expand heaer_binary
  std::memcpy( header_binary.data(), &_header, sizeof(_header) );

  std::vector< std::uint8_t > ret; ret.reserve( command_binary.size() + header_binary.size() );
  ret.insert( ret.end(), header_binary.begin(), header_binary.end() );
  ret.insert( ret.end(), command_binary.begin(), command_binary.end() );

  return ret;
}


bool miya_core_message::import_from_binary( std::vector<std::uint8_t> from_v )
{
  if( from_v.size() >= sizeof(_header) ) std::memcpy( &_header, from_v.data(), sizeof(_header) );


  return false;
}

miya_core_message::miya_core_message() :
  _is_valid( false )
  , _command( MiyaCoreMSG_NONE() )
{
  return;
}

miya_core_command &miya_core_message::get_command()
{
  return _command;
}

miya_core_command::ref miya_core_message::get_command_ref()
{
  return _command.to_ref();
}


/*
bool MiyaChainMessage::is_valid() const
{
  return _is_valid;
}

MiyaChainMessage::MiyaChainMessage() :
  _is_valid(false)
{
  memset( &_header , 0x00 , sizeof(_header) );
}

size_t MiyaChainMessage::payloadLength()
{
	return static_cast<size_t>(_header._payloadLength);
}

const char *MiyaChainMessage::command()
{
	return _header._command;
}

MiyaChainMessage::command_type MiyaChainMessage::commandIndex( const char* command )
  if( memcmp( command , MiyaCoreMSG_INV::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_INV;

  else if( memcmp( command , MiyaCoreMSG_BLOCK::command , chain_MSG_COMMAND_LENGTH )  == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_BLOCK;

  else if( memcmp( command, MiyaCoreMSG_GETBLOCKS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_GETBLOCKS;

  else if( memcmp( command, MiyaCoreMSG_GETDATA::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_GETDATA;

  else if( memcmp( command, MiyaCoreMSG_GETHEADERS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_GETHEADERS;

  else if( memcmp( command , MiyaCoreMSG_HEADERS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_HEADERS;

  else if( memcmp( command, MiyaCoreMSG_MEMPOOL::command , chain_MSG_COMMAND_LENGTH ) == 0 )
	  return MiyaChainMessage::command_type::MiyaCoreMSG_MEMPOOL;

  else
	  return MiyaChainMessage::command_type::MiyaCoreMSG_NOTFOUND;
}


MiyaChainMessage::command_type MiyaChainMessage::get_command_type()
{
  return _command_t;
}


MiyaCoreCommand MiyaChainMessage::payload()
{
	return _payload;
}


void MiyaChainMessage::payload( MiyaCoreCommand targetPayload , const char* comand  )
{
	memcpy( _header._command , comand , sizeof(_header._command) );
	_payload = targetPayload;
}

void MiyaChainMessage::payloadLength( size_t target )
{
	_header._payloadLength = static_cast<uint32_t>(target); // エンディアンん変換はしない
}


bool MiyaChainMessage::import_from_binary( std::vector<unsigned char> from_v )
{
  unsigned char *from = from_v.data();
  std::size_t from_len = from_v.size();

  if( sizeof(_header) > from_len ) return false;

  // header部の取り込み
  std::memcpy( &_header , from , sizeof(_header) );

  if( memcmp(_header._command , MiyaCoreMSG_INV::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : inv" << "\n";
	  struct MiyaCoreMSG_INV payload;
  }

  else if( memcmp( _header._command , MiyaCoreMSG_BLOCK::command , sizeof(_header._command) )  == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : block" << "\n";
	  struct MiyaCoreMSG_BLOCK payload;
  }

  else if( memcmp( _header._command, MiyaCoreMSG_GETBLOCKS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : getblocks" << "\n";
	  struct MiyaCoreMSG_GETBLOCKS payload;
  }

  else if( memcmp( _header._command, MiyaCoreMSG_GETDATA::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : getdata" << "\n";
	  struct MiyaCoreMSG_GETDATA payload;
  }

  else if( memcmp( _header._command, MiyaCoreMSG_GETHEADERS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : getheaders" << "\n";
	  struct MiyaCoreMSG_GETHEADERS payload;

  }

  else if( memcmp( _header._command , MiyaCoreMSG_HEADERS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : headers" << "\n";
	  struct MiyaCoreMSG_HEADERS payload;

  }

  else if( memcmp(_header._command, MiyaCoreMSG_MEMPOOL::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaCoreMSG) : mempool" << "\n";
	  struct MiyaCoreMSG_MEMPOOL paload;

  }

  else if( memcmp(_header._command, MiyaCoreMSG_NOTFOUND::command , sizeof(_header._command)) == 0  )
  {
	  std::cout << "(MiyaCoreMSG) : notfound" << "\n";
	  struct MiyaCoreMSG_NOTFOUND payload;
  }

  else
  {
	  std::cout << "(MiyaCoreMSG) : undefined" << "\n";
  }

  this->_is_valid = true;
  return false;
}


size_t MiyaChainMessage::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> rawPayload; size_t rawPayloadLength;
	rawPayloadLength = exportRawCommand( _header._command , _payload ,&rawPayload );
	this->payloadLength( rawPayloadLength );

	size_t retLength = rawPayloadLength + sizeof(_header);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );

	size_t formatPtr = 0;
	memcpy( (*retRaw).get() , &_header , sizeof(_header) ); formatPtr += sizeof(_header);
	memcpy( (*retRaw).get() + formatPtr , rawPayload.get() , rawPayloadLength ); formatPtr += rawPayloadLength;


	return formatPtr;
}

std::vector<unsigned char> MiyaChainMessage::export_to_bianry() const
{

}

size_t MiyaChainMessage::exportRawCommand( const char* command , MiyaCoreCommand commandBody ,std::shared_ptr<unsigned char> *retRaw )
{
	switch( MiyaChainMessage::commandIndex(command) )
	{
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_INV):
		{
			std::cout << "Hello" << "\n";
		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_BLOCK):
		{

		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_GETBLOCKS):
		{
			std::cout << "msg export with " << _header._command << "\n";
			std::cout << payloadLength() << "\n";
			return (std::get<MiyaCoreMSG_GETBLOCKS>(commandBody)).exportRaw( retRaw );
		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_GETDATA):
		{

		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_GETHEADERS):
		{

		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_HEADERS):
		{

		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_MEMPOOL):
		{

		}
		case static_cast<int>(MiyaCoreCommandIndex::MiyaCoreMSG_NOTFOUND):
		{
			std::cout << "msg export with MiyaCoreMSG_NOTFOUND" << "\n";
			return (std::get<MiyaCoreMSG_NOTFOUND>(commandBody)).exportRaw( retRaw );
		}

		default:
		{
			std::cout << "(コマンド書き出しエラー) 一致するコマンドがありません"  << "\n";
			*retRaw = nullptr;
			return 0;
		}
	}
}



std::shared_ptr<MiyaChainMessage> decode( const json from_j )
{
  if( from_j.contains("command_type") ) return nullptr;
  std::string command_t = from_j["command_type"];

  if( !(from_j.contains("body_b") && from_j["body_b"].is_binary() ) ) return nullptr;
  std::vector<unsigned char> body_b = from_j["body_b"];

  MiyaChainMessage ret;
  return (ret.import_from_binary(body_b) == true ) ? std::make_shared<MiyaChainMessage>(ret) : nullptr;
}

json encode( const MiyaChainMessage &from )
{
  json ret;
}
*/


};
