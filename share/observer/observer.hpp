#ifndef C776E3FF_665F_4DE0_A70F_182B328C349F
#define C776E3FF_665F_4DE0_A70F_182B328C349F


#include <string>
#include <ss_p2p/message.hpp>

#include "boost/asio.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"


using namespace boost::asio;
using namespace boost::uuids;


constexpr unsigned short DEFAULT_EXPIRE_TIME_s = 20/*[seconds]*/;


#endif 
