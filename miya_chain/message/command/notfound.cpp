#include "notfound.h"


namespace miya_chain
{



size_t MiyaChainMSG_NOTFOUND::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
    return _inv.exportRaw( retRaw );
}


};