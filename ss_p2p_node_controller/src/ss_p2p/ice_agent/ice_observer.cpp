#include <ss_p2p/ice_agent/ice_observer.hpp>
#include <ss_p2p/ice_agent/ice_agent.hpp>


namespace ss
{
namespace ice
{


signaling_observer::signaling_observer( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, ss::kademlia::direct_routing_table_controller &d_routing_table_controller ) :
  base_observer( io_ctx )
  , _sender( sender )
  , _ice_sender(ice_sender)
  , _glob_self_ep( glob_self_ep )
  , _d_routing_table_controller( d_routing_table_controller )
{
  return;
}


signaling_request::signaling_request( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, ss::kademlia::direct_routing_table_controller &d_routing_table_controller ) :
  signaling_observer( io_ctx, sender, ice_sender, glob_self_ep, d_routing_table_controller )
{
  return;
}

void signaling_request::init( ip::udp::endpoint &dest_ep, std::string param, json payload ) // ホスト->ピア方向へのNatを開通させるためのダミーメッセージを送信する
{
  this->send_dummy_message( dest_ep ); // NATテーブルに覚えさせるためにダミーメッセージを送信する

  // キャッシュの登録
  _msg_cache.ep = dest_ep;
  _msg_cache.param = param;
  _msg_cache.payload = payload;

  ice_message ice_msg = ice_message::_signaling_(); // シグナリングメッセージの作成
  ice_msg.set_observer_id( signaling_observer::get_id() );
  auto msg_controller = ice_msg.get_signaling_message_controller(); // ice_messageからsignaling_mes_controllerを取得
  msg_controller.set_sub_protocol( ice_message::signaling::sub_protocol_t::request ); // サブプロトコルの設定
  msg_controller.set_dest_endpoint( dest_ep );
  msg_controller.set_src_endpoint( _glob_self_ep );

  std::vector<ip::udp::endpoint> forward_eps = _d_routing_table_controller.collect_endpoint( dest_ep, 3/*適当*/ );
  for( auto itr : forward_eps )
  {
	   _ice_sender.async_ice_send(
       itr
       , ice_msg
	     , std::bind( &signaling_request::on_send_done
		   , this
		   , std::placeholders::_1 )
	  );

	#if SS_VERBOSE
  std::cout << "(signaling_request observer)[init] init message send -> " << itr << "\n";
	#endif
  }

  return;
}

void signaling_request::on_send_done( const boost::system::error_code &ec )
{
  #if SS_VERBOSE
  if( !ec ) ;// std::cout << "(signaling_request observer)[send_done]" << "\n";
  else std::cout << "(signaling_request observer)" << "\x1b[31m" << " send failure" << "\x1b[39m" << "\n";
  #endif
}

void signaling_request::on_traversal_done( const boost::system::error_code &ec )
{
  _done = true;
}

void signaling_request::send_dummy_message( ip::udp::endpoint &ep )
{
  std::string dummy_msg_str = "signaling dummy"; json dummy_msg_json = dummy_msg_str;
  _sender.async_send( ep, "dummy", dummy_msg_json
	  , std::bind( &signaling_request::on_send_done, this, std::placeholders::_1 )
	);
}

json signaling_request::format_request_msg( ip::udp::endpoint &src_ep, ip::udp::endpoint &dest_ep )
{
  json ret;
  ret["protocol"] = "signaling";
  ret["type"] = "request";

  auto exs_dest_ep = extract_endpoint( dest_ep );
  auto exs_src_ep = extract_endpoint( src_ep );
  ret["dest_ipv4"] = exs_dest_ep.first;
  ret["dest_port"] = exs_dest_ep.second;

  ret["src_ipv4"] = exs_src_ep.first;
  ret["src_port"] = exs_src_ep.second;

  ret["ttl"] = DEFAULT_SIGNALING_OPEN_TTL;

  return ret;
}

int signaling_request::income_message( message &msg, ip::udp::endpoint &ep )
{
  if( _done ) return 0; // 処理済みであれば特に何もしない

  ice_message ice_msg( *(msg.get_param("ice_agent")) ); // ここでエラーが発生することは多分ない
  auto msg_controller = ice_msg.get_signaling_message_controller();

  if( msg_controller.get_sub_protocol() == ice_message::signaling::sub_protocol_t::response ) // シグナリング成功レスポンスの場合
  {
	// NAT解決
	_ice_sender.async_send( _msg_cache.ep, _msg_cache.param, _msg_cache.payload
		, std::bind( &signaling_request::on_traversal_done, this, std::placeholders::_1) ); // 疎通後メッセージを送信する
	return 0;
  }

  // ice_messageからip, portの取得
  ip::udp::endpoint dest_ep = msg_controller.get_dest_endpoint();
  if( dest_ep != _ice_sender.get_self_endpoint() ) return 0; // 最終検証 恐らくここで相違が発生することはない

  ice_message ice_res_msg = ice_message::_signaling_();
  auto msg_res_controller = ice_res_msg.get_signaling_message_controller();
  msg_res_controller.set_sub_protocol( ice_message::signaling::sub_protocol_t::response ); // サブプロトコルの設定

  _ice_sender.async_send( _msg_cache.ep, _msg_cache.param,_msg_cache.payload
	  , std::bind( &signaling_request::on_traversal_done, this , std::placeholders::_1 )
	);
  return 0;
}

void signaling_request::print() const
{
  std::cout << "[observer] (signaling_request) " << "<" << _id << ">";
  std::cout << " [ at: "<< signaling_observer::get_expire_time_left() <<" ]";
}


signaling_response::signaling_response( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, direct_routing_table_controller &d_routing_table_controller ) :
  signaling_observer( io_ctx, sender, ice_sender, glob_self_ep, d_routing_table_controller )
{
  return;
}

void signaling_response::init( const boost::system::error_code &ec )
{
  #if SS_VERBOSE
  if( !ec ); // std::cout << "signaling_response::init success" << "\n";
  else std::cout << "(signaling_request observer)" << "\x1b[31m" << " init failure" << "\x1b[39m" << "\n";
  #endif

  if( !ec ) extend_expire_at( 30 ); // 有効期限を30秒延長する
}

int signaling_response::income_message( ss::message &msg, ip::udp::endpoint &ep )
{
  // 一度処理しているため特に処理しない
  extend_expire_at( 20 );
  return 0;
}

void signaling_response::print() const
{
  std::cout << "[observer] (signaling-response) " << "<" << _id << ">";
  std::cout << " [ at: "<< signaling_observer::get_expire_time_left() <<" ]";
}


signaling_relay::signaling_relay( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ip::udp::endpoint &glob_self_ep, direct_routing_table_controller &d_routing_table_controller ) :
  signaling_observer( io_ctx, sender, ice_sender, glob_self_ep, d_routing_table_controller )
{
  return;
}

void signaling_relay::init()
{
  #if SS_VERBOSE
  std::cout << "(signaling_relay)[init]" << "\n";
  #endif

  extend_expire_at( 20 );
}

int signaling_relay::income_message( message &msg, ip::udp::endpoint &ep )
{
  // 一度処理しているため特に処理しない
  extend_expire_at( 20 );
  return 0;
}

void signaling_relay::print() const
{
  std::cout << "[observer] (signaling-relay) " << "<" << _id << ">";
  std::cout << " [ at: "<< signaling_observer::get_expire_time_left() <<" ]";
}


stun_observer::stun_observer( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &d_routing_table_controller ) :
  base_observer( io_ctx )
  , _sender( sender )
  , _ice_sender( ice_sender )
  , _d_routing_table_controller( d_routing_table_controller )
{
  return;
}

binding_request::binding_request( io_context &io_ctx, class sender &sender, class ice_sender &ice_sender, ss::kademlia::direct_routing_table_controller &d_routing_table_controller ) :
  stun_observer( io_ctx, sender, ice_sender, d_routing_table_controller )
  , _timer( io_ctx )
  , _is_timeout( false )
  , _is_handler_called( false )
{
  return;
}

void binding_request::init( std::shared_ptr<stun_server::sr_object> sr )
{
  _sr = sr; // sr objectのセット
  _sr->update_state( stun_server::sr_object::state_t::pending ); // srの実質activate化

  // タイムアウトを予約する
  _timer.expires_from_now( boost::posix_time::seconds(DEFAULT_BINDING_REQUEST_TIMEOUT_s) );
  _timer.async_wait( std::bind( &binding_request::on_timeout, this, std::placeholders::_1) );
}

void binding_request::async_call_sr_handler(std::optional<ip::udp::endpoint> ep )
{
  _io_ctx.post([this, ep](){
		this->_sr->handler( ep );
	  });
  _is_handler_called = true;
}

void binding_request::add_requested_ep( ip::udp::endpoint ep )
{
  // ip::udp::endpoint init_ep( ip::address::from_string("0.0.0.0"), 0 );
  _responses.push_back( std::make_pair(ep, std::nullopt) );
}

void binding_request::add_response( ip::udp::endpoint &src_ep, ip::udp::endpoint response_ep )
{
  auto itr = std::find_if( _responses.begin(), _responses.end(), [src_ep]( const std::pair<ip::udp::endpoint, std::optional<ip::udp::endpoint>> entry ){
		  return entry.first == src_ep;
	  });
  if( itr == _responses.end() ) return;
  (*itr).second = response_ep;
}

int binding_request::income_message( message &msg, ip::udp::endpoint &ep )
{
  // 送信したリクエストのうち3/1が集まって多数決を取る
  ice_message ice_msg( *(msg.get_param("ice_agent")) );

  auto msg_controller = ice_msg.get_stun_message_controller();
  std::optional<ip::udp::endpoint> global_ep = msg_controller.get_global_ep(); // 自身のグローバルipを取得する
  if( global_ep == std::nullopt ) return -1;

  this->add_response( ep, *global_ep ); // レスポンスを保存
  auto cctx = this->global_ep_consensus(); // これまでの応答から有効なグローバルIP(判断に足る応答が不足している場合はfailureが戻る)を取得する
  if( cctx.state == binding_request::consensus_ctx::state_t::on_handling ) return 0; // 判断に足るレスポンス数に達していない

  this->update_sr( cctx ); // sync_getで待機している場合はupdate_stateすると勝手に起きる
  if( _sr->is_async() && !(_is_handler_called) ) this->async_call_sr_handler( cctx.ep ); // 待機状態でなく,非同期でればハンドラを呼び出す

  _timer.cancel(); // 全てのtimeoutハンドラを無効にする
  this->destruct_self(); // 本observerの破棄を許可する

  return 0; // 結果は保存しなくて良い
}

binding_request::consensus_ctx binding_request::global_ep_consensus( bool is_force )
{ // 最も簡易的な実装
  binding_request::consensus_ctx ret; ret.state = binding_request::consensus_ctx::state_t::on_handling;
  // リクエスト件数の3/1にレスポンスがあるか？
  unsigned short response_count = 0;
  for( auto itr : _responses ){
	if( itr.second != std::nullopt ) response_count++; // レスポンスがあったノードのみカウント対象とする
  }
  if( response_count < (_responses.size()/2.5) ) return ret;

  std::vector<ip::udp::endpoint> vli_global_eps; // ローカルアドレスなどを排除する
  for( auto itr : _responses )
  {
	if( itr.second != std::nullopt  // 無効 且つ
		&& !((*(itr.second)).address().is_loopback() ) )
	  { // ループバックアドレスでない場合
		vli_global_eps.push_back( *(itr.second) );
	  }
  }

  if( !is_force ){ // 現在受信している応答のみでグローバルIPを決定する
	if( vli_global_eps.size() < std::max((response_count/3),1) ) return ret; // 判断に必要なレスポンス数に達していない場合
  }
  else{
	if( vli_global_eps.empty() ){
	  ret.state = binding_request::consensus_ctx::state_t::error_done;
	  return ret;
	}
  }

  std::map< ip::udp::endpoint, unsigned short > counter_m;
  std::vector< std::pair<ip::udp::endpoint, unsigned short >> counter_v;
  for( auto itr : vli_global_eps ){
	counter_m[itr]++;
  }

  for( auto [key, value] : counter_m ) counter_v.push_back( std::make_pair(key, value) );
  std::sort( counter_v.begin(), counter_v.end(), []( const auto& _1, auto & _2 ){
		return _1.second > _2.second;
	  });

  // 成功結果を格納
  ret.ep = (counter_v.begin())->first; // 同数一位でも適当に先頭を返す
  ret.state = binding_request::consensus_ctx::state_t::done;

  return ret;
}

void binding_request::on_timeout( const boost::system::error_code &ec )
{
  auto cctx = this->global_ep_consensus(); // タイムアウト時点で有効なノードを取得する
  this->update_sr( cctx );
  if( _sr->is_async() && !(_is_handler_called)/*ハンドラーが呼び出されていない時に限る*/ ) this->async_call_sr_handler( cctx.ep );

  _is_timeout = true; // フラグセットは忘れずに
}

void binding_request::update_sr( const consensus_ctx &cctx )
{
  #if SS_VERBOSE
  std::cout << "[binding_request observer][update state]" << "\n";
  #endif

  // 状態変数の更新
  if( cctx.state == binding_request::consensus_ctx::done )
  {
	_sr->update_state( stun_server::sr_object::done, cctx.ep );
  }
  else if( cctx.state == binding_request::consensus_ctx::error_done )
  {
	_sr->update_state( stun_server::sr_object::notfound );
  }
  else{
	_sr->update_state( stun_server::sr_object::pending );
  }
  return;
}

void binding_request::print() const
{
  std::cout << "[observer] (binding-request) " << "<" << _id << ">";
  std::cout << " [ at: "<< stun_observer::get_expire_time_left() <<" ]";
}


}; // namespace ice
}; // namespace ss
