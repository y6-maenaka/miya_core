#ifndef B2AF92C9_13F7_422C_A0AD_27C4F69FABAF
#define B2AF92C9_13F7_422C_A0AD_27C4F69FABAF


#include <logger/logger.params.hpp>


namespace
{


logger::logger() : 
   _logger_outfile_dir( DEFAULT_LOGGER_OUTFILE_DIR )
  , _logger_outfile_name( DEFAULT_LOGGER_OUTFILE_NAME )
{
  return;
}

template < typename... Args > inline void logger::log( const logger::log_level &ll, Args&&... args ) // 書き出しファイル設定がここで決定するので純粋仮想の方がいいかも
{
  std::string console_prefix_str = get_prefix( ll, true );
  std::string outfile_prefix_str = get_prefix( ll , false );

  // write to local log file 
  auto ofs = get_log_ofs();
  ofs << outfile_prefix_str;
  ((ofs << std::forward<Args>(args) << ' '), ... ); ofs << "\n"; // 本当は改行はここで入れない方がいい

  // write to console
  std::cout << console_prefix_str;
  ((std::cout << std::forward<Args>(args) << ' '), ... ); std::cout << "\n";
}

void logger::set_max_log_level( const logger::log_level &level )
{
  _max_log_level = level;
}

void logger::set_custom_header( std::string target )
{
  _custom_header = target;
}

void logger::set_logger_outfile_name( std::string target )
{
  _logger_outfile_name = target;
}

inline bool logger::should_log()
{
  return true; // temp
}

inline std::string logger::get_prefix( const log_level &ll, bool is_graphical )
{
  std::string ret;
  std::string prefix_fmt = LOGGER_PREFIX_FORMAT;

  std::string serity_str = logger::utils::get_serity_str( ll );
  if( is_graphical ){
	std::string serity_color_code_str = logger::utils::get_serity_color_code( ll );
	serity_str = serity_color_code_str + serity_str + "\x1b[39m"; // 後で修正する
  }

  const auto current_time_str = logger::utils::get_current_time_str( LOGGER_TIME_FORMAT );

  ret = logger::utils::replace( prefix_fmt, "{SERITY}", serity_str );
  ret = logger::utils::replace( ret, "{TIME}", current_time_str );
  ret = logger::utils::replace( ret , "{CUSTOM}", _custom_header );

  return ret;
}

std::ofstream logger::get_log_ofs()
{
  std::ofstream ret;

  std::string current_time_t = logger::utils::get_current_time_str( LOGGER_OUTFILE_TIME_FORMAT );
  std::string logger_outfile_fmt = LOGGER_OUTFILE_FORMAT;
  std::string logger_outfile_name = logger::utils::replace( logger_outfile_fmt, "{EXE}", _logger_outfile_name );
  logger_outfile_name = logger::utils::replace( logger_outfile_name, "{DATE}", current_time_t );

  std::stringstream logger_outfile_path_ss; logger_outfile_path_ss << _logger_outfile_dir << "/" << logger_outfile_name;

  ret.open( logger_outfile_path_ss.str() , std::ios::app );
  return std::move(ret);
}

inline std::string logger::utils::replace( std::string base, std::string rpl_from, std::string rpl_to )
{
  std::string ret = base;
  std::size_t idx = 0;
  while( (idx = ret.find(rpl_from, idx)) != std::string::npos ){
	ret.replace( idx, rpl_from.length(), rpl_to );
	idx += rpl_to.size();
  }
  
  return ret;
}

inline std::string logger::utils::get_current_time_str( std::string fmt )
{
  std::time_t current_time_t = std::time(nullptr);
  struct std::tm* time_info = std::localtime(&current_time_t);

  char temp[256];
  std::strftime( temp, sizeof(temp), fmt.c_str(), time_info );

  std::string ret(temp);
  return ret;
};

inline std::string logger::utils::get_color_code( const logger::utils::bg_color &bc )
{
  switch( bc )
  {
	case logger::utils::bg_color::RESET : return "\x1b[0m";
	case logger::utils::bg_color::BG_RED : return "\x1b[41m";
	case logger::utils::bg_color::BG_GREEN : return "\x1b[42m";
	case logger::utils::bg_color::BG_YELLOW : return "\x1b[43m";
	case logger::utils::bg_color::BG_BLUE : return "\x1b[44m";
	case logger::utils::bg_color::BG_MAGENTA : return "\x1b[45m";
	case logger::utils::bg_color::BG_GREY : return "\x1b[46m";
  }
  return "";
}

inline std::string logger::utils::get_color_code( const logger::utils::fg_color &fc )
{
  switch( fc )
  {
	case logger::utils::fg_color::RESET : return "\x1b[0m";
	case logger::utils::fg_color::FG_RED : return "\x1b[31m";
	case logger::utils::fg_color::FG_GREEN : return "\x1b[32m";
	case logger::utils::fg_color::FG_YELLOW : return "\x1b[33m";
	case logger::utils::fg_color::FG_BLUE : return "\x1b[34m";
	case logger::utils::fg_color::FG_MAGENTA : return "\x1b[35m";
	case logger::utils::fg_color::FG_GREY : return "\x1b[36m";
  }
  return "";
}

inline std::string logger::utils::get_serity_str( const logger::log_level &ll )
{
  std::string ret;

  switch( ll )
  {
	case logger::log_level::EMERGENCY :
	  {
		ret = "EMERGENCY";
		break;
	  }
	case logger::log_level::ALERT : 
	  {
		ret = "ALERT";
		break;
	  }
	case logger::log_level::CRITICAL : 
	  {
		ret = "CRITICAL";
		break;
	  }
	case logger::log_level::ERROR :
	  {
		ret = "ERROR";
		break;
	  }
	case logger::log_level::WARNING :
	  {
		ret = "WARNING";
		break;
	  }
	case logger::log_level::NOTICE :
	  {
		ret = "NOTICE";
		break;
	  }
	case logger::log_level::INFO :
	  {
		ret = "INFO";
		break;
	  }
	case logger::log_level::DEBUG :
	  {
		ret = "DEBUG";
		break;
	  }
	default : 
	  {
		ret = "";
		break;
	  }
  }
  return ret;
}

inline std::string logger::utils::get_serity_color_code( const logger::log_level &ll )
{
  std::stringstream ret_ss;
  switch( ll )
  {
	case logger::log_level::EMERGENCY :
	  ret_ss << get_color_code(logger::utils::bg_color::BG_RED);
	  break;
	case logger::log_level::ALERT :
	  ret_ss << get_color_code(logger::utils::bg_color::BG_YELLOW);
	  break;
	case logger::log_level::CRITICAL : 
	  ret_ss << get_color_code(logger::utils::bg_color::BG_YELLOW);
	  break;
	default :
	  break;
  }

  switch( ll )
  {
	case logger::log_level::CRITICAL : 
	  ret_ss << get_color_code( logger::utils::fg_color::FG_RED );
	  break;
	case logger::log_level::ERROR :
	  ret_ss << get_color_code( logger::utils::fg_color::FG_RED );
	  break;
	case logger::log_level::WARNING : 
	  ret_ss << get_color_code( logger::utils::fg_color::FG_YELLOW );
	  break;
	case logger::log_level::INFO :
	  ret_ss << get_color_code( logger::utils::fg_color::FG_GREEN );
	  break;
	case logger::log_level::DEBUG : 
	  ret_ss << get_color_code( logger::utils::fg_color::FG_MAGENTA );
	  break;
	default :
	  break;
  }

  return ret_ss.str();
}


}; // namesapce ""


#endif 
