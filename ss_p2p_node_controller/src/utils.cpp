#include <utils.hpp>
#include <sys/ioctl.h>


namespace ss
{


std::pair< std::shared_ptr<unsigned char>, std::size_t > endpoint_to_binary( ip::udp::endpoint &ep ) noexcept
{
  const auto addr_str = ep.address().to_string();
  const auto port = ep.port();

  std::shared_ptr<unsigned char> addr_binary = std::shared_ptr<unsigned char>( new unsigned char[addr_str.size() + sizeof(port)] );
  std::size_t cpyOffset = 0;
  std::memcpy( addr_binary.get() + cpyOffset, addr_str.data(), addr_str.size() ); cpyOffset += addr_str.size();
  std::memcpy( addr_binary.get() + cpyOffset, &port, sizeof(port) ); cpyOffset += sizeof(port);

  return std::make_pair( addr_binary, cpyOffset );
}

std::string endpoint_to_str( ip::udp::endpoint &ep )
{
  std::string ret = ep.address().to_string() + ":" + std::to_string(ep.port());
  return ret;
}

ip::udp::endpoint str_to_endpoint( std::string &ep_str )
{
  std::string ipv4 = ep_str.substr( 0, ep_str.find(":") );
  unsigned short port = std::stoi(ep_str.substr(ep_str.find(':') + 1));

  return ip::udp::endpoint( ip::address::from_string(ipv4), port );
}

std::pair<std::string, std::uint16_t> extract_endpoint( ip::udp::endpoint &ep )
{
  return std::make_pair( ep.address().to_string(), static_cast<std::uint16_t>(ep.port()) );
}

ip::udp::endpoint addr_pair_to_endpoint( std::string ip, std::uint16_t port )
{
  return ip::udp::endpoint( ip::address::from_string(ip), port );
}

ip::udp::endpoint generate_random_endpoint()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis_ip(1, 255);
  std::uniform_int_distribution<> dis_port(1024, 65535);

  std::uint8_t byte_1 = dis_ip(gen);
  std::uint8_t byte_2 = dis_ip(gen);
  std::uint8_t byte_3 = dis_ip(gen);
  std::uint8_t byte_4 = dis_ip(gen);

  boost::asio::ip::address_v4 random_ip((byte_1 << 24) | (byte_2 << 16) | (byte_3 << 8) | byte_4);
  unsigned short random_port = dis_port(gen);

  return ip::udp::endpoint( random_ip, random_port );
}

std::size_t get_console_width()
{
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  return ws.ws_col;
}


}; // namespace ss
