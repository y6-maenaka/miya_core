#ifndef A2B382DF_97D4_4089_9882_F64F2CFC3E58
#define A2B382DF_97D4_4089_9882_F64F2CFC3E58


#include <span>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>

#include <json.hpp>

using json = nlohmann::json;


namespace ss
{


using encoded_message = std::span<char const>;


class ss_message;


class message
{
  friend ss_message;
public:
  using app_id = std::array<char, 8>;
  message( app_id id );
  message( json from );

  void set_app_id( app_id );
  std::shared_ptr<json> get_param( std::string param );
  void set_param( std::string key, const json value );
  [[ nodiscard ]] bool is_contain_param( std::string param );

  using encoded_message = std::span<char>;
  // static message (request)(app_id &id);
  static std::vector<std::uint8_t> encode( const message &from );
  static message decode( std::vector<std::uint8_t> from ); // 生メッセージのデコード

  json& operator()();
  void print();
private:
  json _body;

};


};


#endif
