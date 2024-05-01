#ifndef A94556B1_4F5D_453A_9EF5_8260CF7F0E7C
#define A94556B1_4F5D_453A_9EF5_8260CF7F0E7C


#include "message.hpp"


namespace chain
{


miya_core_command::miya_core_command( const T c ) : 
  _command(c)
  , _is_valid(false)
{
  return;
}

template <typename T > void miya_core_command::set_command( const T c )
{
  _command = c;
  _command_t = get_command_type(_command);
}

std::vector<std::uint8_t> miya_core_command::export_to_binary() const
{
  if( !(this->is_valid()) ) return std::vector<std::uint8_t>();

   
  return std::vector<std::uint8_t>();
}

bool miya_core_command::is_valid() const
{
  return _is_valid;
}


miya_core_command::command_type miya_core_command::get_command_type( const command &c )
{
  if( std::get_if<MiyaCoreMSG_INV>(&c) ) return command_type::INV;
  else if( std::get_if<MiyaCoreMSG_GETBLOCKS>(&c) ) return command_type::GETBLOCKS;
  else if( std::get_if<MiyaCoreMSG_BLOCK>(&c) ) return command_type::BLOCK;
  else if( std::get_if<MiyaCoreMSG_GETDATA>(&c) ) return command_type::GETDATA;
  else if( std::get_if<MiyaCoreMSG_GETHEADERS>(&c) ) return command_type::GETHEADERS;
  else if( std::get_if<MiyaCoreMSG_HEADERS>(&c) ) return command_type::HEADERS;
  else if( std::get_if<MiyaCoreMSG_MEMPOOL>(&c) ) return command_type::MEMPOOL;
  else if( std::get_if<MiyaCoreMSG_NOTFOUND>(&c) ) return command_type::NOTFOUND;
  else return command_type::INVALID;
}


};


#endif 
