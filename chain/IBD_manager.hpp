#ifndef C0E4EEF9_7718_4C58_A924_5F8130701A13
#define C0E4EEF9_7718_4C58_A924_5F8130701A13


#include <chain/chain_sync_manager.hpp>
#include <vector>
#include <memory>


namespace chain
{

class IBD_chain_sync_manager : public : chain_sync_manager
{
private:
    // std::vector< block::header::ref > _candidate_chain;

public:
    void sync_start( );
};


};


#endif 

// chain_sync_managerを継承して一部メソッドをオーバーライドすることで,IBDに適したobserver制御を行う
