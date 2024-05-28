#ifndef EA2276B8_ADB8_45BA_A32B_10BB082E3CE9
#define EA2276B8_ADB8_45BA_A32B_10BB082E3CE9


#include <string>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <random>

#include "boost/asio.hpp"


using namespace boost::asio;


#ifdef __linux__
/*
namespace std
{
  template<>
  struct hash<boost::asio::ip::address_v4> {
	std::size_t operator()( const boost::asio::ip::address_v4 &addr ) const {
	  return static_cast<std::size_t>(addr.to_ulong());
	}
  };
}
*/
#endif


namespace ss
{


std::pair< std::shared_ptr<unsigned char>, std::size_t > endpoint_to_binary( ip::udp::endpoint &ep ) noexcept;
std::string endpoint_to_str( const ip::udp::endpoint &ep ); // encode endpoint
ip::udp::endpoint str_to_endpoint( std::string &ep_str ); // decode endpoint
std::pair<std::string, std::uint16_t> extract_endpoint( ip::udp::endpoint &ep );
ip::udp::endpoint addr_pair_to_endpoint( std::string ip, std::uint16_t port );
ip::udp::endpoint generate_random_endpoint();

std::size_t get_console_width();

}; // namespace ss


#endif 


