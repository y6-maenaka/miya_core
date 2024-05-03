#include "core.hpp"


namespace core
{


core_context::core_context( std::string path_to_ecdsa_pem )
{
  if( !(this->load_ecdsa_key( path_to_ecdsa_pem )) ) return;
}

std::string core_context::get_ecdsa_pem_pass()
{
  return _ecdsa_pem_pass;
}

bool core_context::load_ecdsa_key( std::string path_to_ecdsa_pem )
{
  nlohmann::json pem_pass_j;
  std::ifstream input( path_to_ecdsa_pem );
  input >> pem_pass_j;

  std::vector<std::uint8_t> pem_pass_v;
  if( !(pem_pass_j.contains("ecdsa_pem_pass")) ) return false;

  _ecdsa_pem_pass = pem_pass_j["ecdsa_pem_pass"];
  return true;
}


core::core( std::shared_ptr<io_context> io_ctx ) :
  _io_ctx( io_ctx )
  , _context( "../.config/wallet.json" )
  , _message_broker( *this )
  // , _chain_manager( "path/to/l_chain_state" )
{
  return;
}

void core::start()
{
  // IBD ibd( _chain_manager );
  // if( !(ibd.start()) ) return;

  
}

core_context &core::get_context()
{
    return _context;
}


};
