#include "message.h"


namespace chain
{

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


unsigned short MiyaChainMessage::commandIndex()
{
	return MiyaChainMessage::commandIndex( _header._command );
}

int MiyaChainMessage::commandIndex( const char* command )
{
	if( memcmp( command , MiyaChainMSG_INV::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_INV);

	else if( memcmp( command , MiyaChainMSG_BLOCK::command , chain_MSG_COMMAND_LENGTH )  == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK);

	else if( memcmp( command, MiyaChainMSG_GETBLOCKS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETBLOCKS);

	else if( memcmp( command, MiyaChainMSG_GETDATA::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETDATA);

	else if( memcmp( command, MiyaChainMSG_GETHEADERS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETHEADERS);

	else if( memcmp( command , MiyaChainMSG_HEADERS::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_HEADERS);

	else if( memcmp( command, MiyaChainMSG_MEMPOOL::command , chain_MSG_COMMAND_LENGTH ) == 0 )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_MEMPOOL);

	else if( memcmp( command, MiyaChainMSG_NOTFOUND::command , chain_MSG_COMMAND_LENGTH ) == 0  )
		return static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_NOTFOUND);

	else
		return -1;

}


MiyaChainMessage::command_type MiyaChainMessage::get_command_type()
{
  return _command_t;
}


MiyaChainCommand MiyaChainMessage::payload()
{
	return _payload;
}


void MiyaChainMessage::payload( MiyaChainCommand targetPayload , const char* comand  )
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
  memcpy( &_header , from , sizeof(_header) );

  if( memcmp(_header._command , MiyaChainMSG_INV::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : inv" << "\n";
	  struct MiyaChainMSG_INV payload;
  }

  else if( memcmp( _header._command , MiyaChainMSG_BLOCK::command , sizeof(_header._command) )  == 0 )
  {
	  std::cout << "(MiyaChainMSG) : block" << "\n";
	  struct MiyaChainMSG_BLOCK payload;
  }

  else if( memcmp( _header._command, MiyaChainMSG_GETBLOCKS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : getblocks" << "\n";
	  struct MiyaChainMSG_GETBLOCKS payload;
  }

  else if( memcmp( _header._command, MiyaChainMSG_GETDATA::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : getdata" << "\n";
	  struct MiyaChainMSG_GETDATA payload;
  }

  else if( memcmp( _header._command, MiyaChainMSG_GETHEADERS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : getheaders" << "\n";
	  struct MiyaChainMSG_GETHEADERS payload;

  }
  
  else if( memcmp( _header._command , MiyaChainMSG_HEADERS::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : headers" << "\n";
	  struct MiyaChainMSG_HEADERS payload;
	  
  }

  else if( memcmp(_header._command, MiyaChainMSG_MEMPOOL::command , sizeof(_header._command) ) == 0 )
  {
	  std::cout << "(MiyaChainMSG) : mempool" << "\n";
	  struct MiyaChainMSG_MEMPOOL paload;
	  
  }

  else if( memcmp(_header._command, MiyaChainMSG_NOTFOUND::command , sizeof(_header._command)) == 0  )
  {
	  std::cout << "(MiyaChainMSG) : notfound" << "\n";
	  struct MiyaChainMSG_NOTFOUND payload;
  }

  else
  {
	  std::cout << "(MiyaChainMSG) : undefined" << "\n";
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

size_t MiyaChainMessage::exportRawCommand( const char* command , MiyaChainCommand commandBody ,std::shared_ptr<unsigned char> *retRaw )
{

	switch( MiyaChainMessage::commandIndex(command) )
	{
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_INV):
		{
			std::cout << "Hello" << "\n";
		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK):
		{

		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETBLOCKS):
		{
			std::cout << "msg export with " << _header._command << "\n";
			std::cout << payloadLength() << "\n";
			return (std::get<MiyaChainMSG_GETBLOCKS>(commandBody)).exportRaw( retRaw );
		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETDATA):
		{

		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_GETHEADERS):
		{

		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_HEADERS):
		{

		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_MEMPOOL):
		{

		}
		case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_NOTFOUND):
		{
			std::cout << "msg export with MiyaChainMSG_NOTFOUND" << "\n";
			return (std::get<MiyaChainMSG_NOTFOUND>(commandBody)).exportRaw( retRaw );
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


};
