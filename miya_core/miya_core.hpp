#ifndef F720D5DF_D04A_420E_950E_112986BF5BF9
#define F720D5DF_D04A_420E_950E_112986BF5BF9


#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <span>

#include <unistd.h>
#include <ss_p2p/message_pool.hpp>
#include <crypto_utils/w_sha/sha.h>
#include "../share/json.hpp"


using json = nlohmann::json;


namespace miya_core
{


struct miya_core_context 
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
    miya_core_context( std::string path_to_ecdsa_pem );
    std::string get_ecdsa_pem_pass();
    bool load_ecdsa_key( std::string path_to_ecdsa_pem );
};


// MiyaCore全体統合モジュール
class miya_core
{
private:
    miya_core_context _context;

public:
    miya_core();

    miya_core_context &get_context();
    void on_income_message( ss::message_pool::_message_ msg ); // 外部からメッセージが到着した時のハンドラ
};


};


#endif // F720D5DF_D04A_420E_950E_112986BF5BF9
