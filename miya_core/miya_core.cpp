#include "miya_core.hpp"

#include "../share/json.hpp"
#include "../ss_p2p_node_controller/include/ss_p2p/message_pool.hpp"


namespace miya_core
{


miya_core_context::miya_core_context( std::string path_to_ecdsa_pem )
{
  if( !(this->load_ecdsa_key( path_to_ecdsa_pem )) ) return;
}

std::string miya_core_context::get_ecdsa_pem_pass()
{
  return _ecdsa_pem_pass;
}

bool miya_core_context::load_ecdsa_key( std::string path_to_ecdsa_pem )
{
  nlohmann::json pem_pass_j;
  std::ifstream input( path_to_ecdsa_pem );
  input >> pem_pass_j;

  std::vector<std::uint8_t> pem_pass_v;
  if( !(pem_pass_j.contains("ecdsa_pem_pass")) ) return false;

  _ecdsa_pem_pass = pem_pass_j["ecdsa_pem_pass"];
  return true;
}


miya_core::miya_core() :
  _context( "../.config/wallet.json" )
{
  return;
}

miya_core_context &miya_core::get_context()
{
    return _context;
}

void miya_core::on_income_message( ss::message_pool::_message_ msg )
{
  unsigned char temp[10]; memcpy( temp, "HelloWorld", 10 );
  std::shared_ptr<unsigned char> ret;
  openssl_wrapper::sha::W_SHA::hash( temp, 10, &ret, "sha1" );

  std::cout << "(recv) <- " << msg.src_ep << "\n";
  std::cout << "message received" << "\n";
}


};
