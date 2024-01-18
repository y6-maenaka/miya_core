#include "miya_core.h"

#include "../shared_components/json.hpp"


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



};
