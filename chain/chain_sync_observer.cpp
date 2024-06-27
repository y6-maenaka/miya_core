#include <chain/chain_sync_observer.hpp>


namespace chain
{


chain_sync_observer::chain_sync_observer( io_context &io_ctx, std::string t_name ) :
  base_observer( io_ctx, t_name )
{
  return;
}

getdata_observer::getdata_observer( io_context &io_ctx, std::string t_name ) : 
  chain_sync_observer( io_ctx, t_name )
{
  return;
}

void getdata_observer::init( ss::peer::ref peer_ref, const BLOCK_ID &block_id )
{
  std::vector< inv::ref > request_hash_v;
  request_hash_v.push_back( std::make_shared<inv>( std::make_pair(inv::type_id::MSG_BLOCK, block_id) ) );

  auto request_cmd = std::make_shared<miya_core_command>( MiyaCoreMSG_GETDATA() );
  const auto exported_cmd = request_cmd->export_to_binary();

  peer_ref->send( exported_cmd ); // リクエストコマンド(getdata)の送信
}


};
