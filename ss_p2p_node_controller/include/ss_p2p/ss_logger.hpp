#ifndef D8FB837F_FA5A_462D_B238_4ECAEDF22DE5
#define D8FB837F_FA5A_462D_B238_4ECAEDF22DE5


#include <logger/logger.hpp>

#include "boost/asio.hpp"


#if !defined(SS_LOGGING_DISABLE) \
  || !defined(SS_SYSTEM_LOG_OUTPUT_FILENAME) \
  || !defined(SS_PACKET_LOG_OUTPUT_FILENAME)

#define SS_LOGGER_SYSTEM_OUTFILE_NAME "ss_system"
#define SS_LOGGER_PACKET_OUTFILE_NAME "ss_pakcet"

#endif


using namespace boost::asio;


namespace ss
{


class ss_logger 
{
private:
  logger _logger; // 通常(system)ログの記録用
  logger _packet_logger; // パケットログの記録用

public:
  enum packet_direction
  {
	INCOMING
	  , OUTGOING
  };

  template <typename ... Args > void log( const logger::log_level &ll, Args&& ... args );
  template <typename... Args > void log_packet( const logger::log_level &ll, const packet_direction &pd, const ip::udp::endpoint &ep, Args&& ... args );

  ss_logger(){
	_packet_logger.set_logger_outfile_name( SS_LOGGER_PACKET_OUTFILE_NAME ); // change log out filename
	
	_packet_logger.set_custom_header("[SS_P2P]");
	_logger.set_custom_header("[SS_P2P]");
  }
};


}; // namespace ss


#include "./ss_logger.impl.hpp"


#endif
