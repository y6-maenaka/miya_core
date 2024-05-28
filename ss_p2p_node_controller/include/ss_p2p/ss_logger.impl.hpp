#ifndef E5188D95_D8A9_4E6C_A813_A3A531B512A3
#define E5188D95_D8A9_4E6C_A813_A3A531B512A3


#include <utils.hpp>


namespace ss
{


template <typename ... Args > void ss_logger::log( const logger::log_level &ll, Args&& ... args )
{
  _logger.log( ll, std::forward<Args>(args)... );

  _logger.set_custom_header("[SS_P2P]");
  _packet_logger.set_custom_header("[SS_P2P]");
}

template <typename... Args > void ss_logger::log_packet( const logger::log_level &ll, const packet_direction &pd, const ip::udp::endpoint &ep, Args&& ... args )
{
  std::string ep_str = endpoint_to_str(ep);
  if( pd == ss_logger::packet_direction::INCOMING ) return _packet_logger.log( ll, "(receive):" , ep_str.c_str() ,std::forward<Args>(args)... );
  else return _packet_logger.log( ll, "(send):", ep_str.c_str() ,std::forward<Args>(args)... );
}

/*
 [INFO][22:10:54] [SEND](UDP) 
*/


};


#endif
