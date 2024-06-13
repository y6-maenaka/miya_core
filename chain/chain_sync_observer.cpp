#include <chain/chain_sync_observer.hpp>


namespace chain
{


chain_sync_observer::chain_sync_observer( io_context &io_ctx, std::string t_name ) :
  base_observer( io_ctx, t_name )
{
  return;
}


};
