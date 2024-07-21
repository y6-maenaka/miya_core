#include <chain/chain_sync_observer.hpp>


namespace chain
{


chain_sync_observer::chain_sync_observer( io_context &io_ctx, std::string t_name, const ss::base_observer::id &id_from ) :
  base_observer( io_ctx, t_name, id_from )
{
  return;
}

getdata_observer::getdata_observer( io_context &io_ctx
, ss::peer::ref peer_ref, const BLOCK_ID &block_id
, const ss::base_observer::id &id_from, std::string t_name ) : 
  chain_sync_observer( io_ctx, t_name, id_from )
  , _peer( peer_ref )
  , _base_id( block_id )
{
  return;
}

void getdata_observer::init()
{ // invで受信したオブジェクトリストから特定のデータオブジェクトを取得する為に使用. 既にチェーンに取り込まれたトランザクションなどは要求できない
  std::vector< inv::ref > request_hash_v;
  request_hash_v.push_back( std::make_shared<inv>( std::make_pair(inv::type_id::MSG_BLOCK, _block_id) ) );

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


};
