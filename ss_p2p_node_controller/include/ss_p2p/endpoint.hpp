#ifndef C6C56BF4_AD2D_45F5_986A_A870032DD592
#define C6C56BF4_AD2D_45F5_986A_A870032DD592

#include "boost/asio.hpp"

using namespace boost::asio;

namespace ss
{


struct ip_address
{
  union addr_t
  {
	ip::address_v4 v4;
	ip::address_v6 v6;
  };

  addr_t addr;
  bool v4:1;
};


struct endpoint
{
  ip_address addr;
  std::uint16_t port;
};


}; // namesapce ss


#endif 


