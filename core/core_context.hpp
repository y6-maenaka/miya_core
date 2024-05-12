#ifndef D375D048_D21A_46D6_AD00_1EB460674DE7
#define D375D048_D21A_46D6_AD00_1EB460674DE7


#include <string>
#include <fstream>

#include <json.hpp>


namespace core
{


struct core_context
{
private:
    std::string _ecdsa_pem_pass;

    struct mining_info
    {
        std::uint32_t _version;
        struct coinbase
        {
            std::size_t _mining_reward;
            std::string _mining_text; // conibaseに埋め込むテキスト
        } _coinbase;
    };

public:
    core_context( std::string path_to_ecdsa_pem );
    std::string get_ecdsa_pem_pass();
    bool load_ecdsa_key( std::string path_to_ecdsa_pem );
};


}; // namesapce core


#endif
