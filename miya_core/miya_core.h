#ifndef F720D5DF_D04A_420E_950E_112986BF5BF9
#define F720D5DF_D04A_420E_950E_112986BF5BF9

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include <unistd.h>


namespace miya_core
{



struct MiyaCoreContext
{
private:
    struct PemPass
    {
        size_t _passSize;
        std::shared_ptr<unsigned char> _pass;
    } _pemPass;

    struct
    {
        uint32_t _version;
        struct
        {
            size_t _miningReward;
            std::string _miningText;
        } _coinbaseCTX;
    } _miyaCore;

public:
    bool loadECDSAKey( std::string pathToECDSAPem );

    size_t pemPass( std::shared_ptr<unsigned char> *ret );
};




// MiyaCore全体統合モジュール
class MiyaCore
{
private:
    std::shared_ptr<MiyaCoreContext> _context;

public:
    MiyaCore();

    const std::shared_ptr<MiyaCoreContext> context();
};



};

#endif // F720D5DF_D04A_420E_950E_112986BF5BF9
