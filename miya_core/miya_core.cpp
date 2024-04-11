#include "miya_core.hpp"

#include "../shared_components/json.hpp"
#include "../ss_p2p_node_controller/include/ss_p2p/message_pool.hpp"


namespace miya_core
{


bool MiyaCoreContext::loadECDSAKey( std::string pathToECDSAPem )
{
    nlohmann::json pemPassJson;
    std::ifstream input(pathToECDSAPem);
    input >> pemPassJson;

    std::vector<uint8_t> pemPassVector;
    if( !(pemPassJson.contains("ecdsa_pem_pass")) ) return false;

    std::string pemPassString = pemPassJson["ecdsa_pem_pass"];
    _pemPass._passSize = pemPassString.size();
    _pemPass._pass = std::shared_ptr<unsigned char>( new unsigned char[_pemPass._passSize] );
    memcpy( _pemPass._pass.get(), pemPassString.c_str(), _pemPass._passSize );

    return true;
}


size_t MiyaCoreContext::pemPass( std::shared_ptr<unsigned char> *ret )
{
    *ret = _pemPass._pass;
    return _pemPass._passSize;
}


MiyaCore::MiyaCore()
{
    _context = std::make_shared<MiyaCoreContext>();
    _context->loadECDSAKey( std::string("../.config/wallet.json") );
    return ;
}


const std::shared_ptr<MiyaCoreContext> MiyaCore::context()
{
    return _context;
}

void MiyaCore::on_income_message( ss::message_pool::_message_ msg )
{
  unsigned char temp[10]; memcpy( temp, "HelloWorld", 10 );
  std::shared_ptr<unsigned char> ret;
  openssl_wrapper::sha::W_SHA::hash( temp, 10, &ret, "sha1" );

  std::cout << "(recv) <- " << msg.src_ep << "\n";
  std::cout << "message received" << "\n";
}


};
