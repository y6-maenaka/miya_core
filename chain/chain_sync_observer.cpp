#include <chain/chain_sync_observer.hpp>


namespace chain
{


chain_sync_observer::chain_sync_observer( io_context &io_ctx, ss::peer::ref target_peer, std::string t_name, const ss::base_observer::id &id_from ) :
  base_observer( io_ctx, t_name, id_from )
  , _peer( target_peer )
{
  return;
}

getdata_observer::getdata_observer( io_context &io_ctx
, ss::peer::ref peer_ref, const block_id &block_id
, const ss::base_observer::id &id_from, std::string t_name ) : 
  chain_sync_observer( io_ctx, peer_ref, t_name, id_from )
  , _base_id( block_id )
{
  return;
}

void getdata_observer::init()
{ // invで受信したオブジェクトリストから特定のデータオブジェクトを取得する為に使用. 既にチェーンに取り込まれたトランザクションなどは要求できない
  std::vector< inv::ref > request_hash_v;
  request_hash_v.push_back( std::make_shared<inv>( std::make_pair(inv::type_id::MSG_BLOCK, _base_id) ) );

  auto request_cmd = std::make_shared<miya_core_command>( MiyaCoreMSG_GETDATA() ); // リクエストメッセージの作成
  const auto exported_cmd = request_cmd->export_to_binary(); // メッセージをダンプ

  _peer->send( exported_cmd ); // リクエストコマンド(getdata)の送信
  std::cout << "getdata_observer waitinig response. expire time :: " << (_expire_at - std::time(nullptr)) << "\n"; // DEBUG
}

void getdata_observer::on_receive( const ss::peer::ref &peer_ref )
{
  return;
}

void getdata_observer::on_success( const ss::peer::ref &peer_ref )
{
  // さらにチェーンの延長をするかどうか?
  return;
}

void getdata_observer::on_notfound( const ss::peer::ref &peer_ref )
{
  return;
}

void block_observer::on_receive( const ss::peer::ref &peer_ref, block::ref block_ref )
{
 // 1. blockの検証
  // 2. prev_blockがローカルチェーンに繋がるか否か( height, id )
  //   (yes) : chain_sync_managerにmerge依頼を出す
  //   (no) : prev_block_idをキーにget_block依頼を出す
}

getblocks_observer::getblocks_observer( io_context &io_ctx, ss::peer::ref target_peer, const block_id start_blkid, const block_id stop_blkid
, getblocks_observer::on_done_notify_func notify_func, std::string t_name ) :
  chain_sync_observer( io_ctx, target_peer, t_name )
  , _start_blkid( start_blkid ), _stop_blkid( stop_blkid )
  , _notify_func( notify_func )
{
  return;
}

void getblocks_observer::request()
{
  struct MiyaCoreMSG_GETBLOCKS getblocks_msg( _start_blkid, _stop_blkid );
  auto msg = getblocks_msg.export_to_binary<std::uint8_t>();

  _peer->send( msg );
  return;
}

bool getblocks_observer::is_correspond_inv( inv::ref cmd ) const
{
  return false;
}

void getblocks_observer::income_inv( inv::ref cmd )
{
  return;
}


};
