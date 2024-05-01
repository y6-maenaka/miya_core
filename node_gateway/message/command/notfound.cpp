#include "notfound.hpp"


namespace chain
{


size_t MiyaCoreMSG_NOTFOUND::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
    return _inv.exportRaw( retRaw );
}


};
