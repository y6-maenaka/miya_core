#include <core/net_processing.hpp>


namespace core
{


net_processing::net_processing( chain::chain_manager &chain_manager, chain::mempool &mempool, ss::message_pool &message_pool ) : 
  _chain_manager( chain_manager )
  , _mempool( mempool )
  , _message_pool( message_pool )
{
  _message_hub = &(_message_pool.get_message_hub());
}

void net_processing::start()
{
  // return _message_hub->start( std::bind( &net_processing::process_message, this, std::placeholders::_1) );
}

void net_processing::process_message( ss::ss_message income_msg )
{
  // auto peer_ref = make_peer();
}


};
