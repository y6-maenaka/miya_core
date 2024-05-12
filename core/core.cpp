#include "core.hpp"


namespace core
{


core::core() : 
  _context( "../.config/wallet.json" )
  , _message_broker( *this )
  // , _chain_manager( "path/to/l_chain_state", _context, )
{
  return;
}

void core::start()
{
  // IBD ibd( _chain_manager );
  // if( !(ibd.start()) ) return;
}

void core::on_update_chain()
{

}

core_context &core::get_context()
{
    return _context;
}


};
