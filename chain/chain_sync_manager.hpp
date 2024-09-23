#ifndef AF617A30_B298_41BE_8904_BC1CE6F1D871
#define AF617A30_B298_41BE_8904_BC1CE6F1D871


#include <chain/chain_sync_observer.hpp>
#include <chain/block/block.params.hpp>
// #include <chain/block/block_header.hpp>
#include <chain/chain.hpp>
#include <ss_p2p/observer.hpp>
#include <ss_p2p/observer_strage.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/command/inv.hpp>
#include <node_gateway/message/command/notfound.hpp>
#include <node_gateway/message/command/block.hpp>

#include "boost/asio.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/multi_index_container.hpp"
#include "boost/multi_index/tag.hpp"
#include "boost/multi_index/identity.hpp"
#include "boost/multi_index/indexed_by.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/sequenced_index.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/global_fun.hpp"
#include "boost/multi_index/mem_fun.hpp"

#include <functional>
#include <type_traits>

using namespace boost::asio;



namespace chain
{


class block;

struct by_peer_id
{
  template<typename T> using key_type = ss::peer::id;
};


template <typename T> struct CHAIN_SYNC_OBSERVER_STRAGE_POLICY
{

  // SFINAEで実装(テンプレートの書き換えに失敗してもエラーにならない)
  template < typename U, typename = void > struct has_peer_id : std::false_type{}; // get_peer_idメソッドがあるかどうかを判定, 初期でfalse_typeを継承
  template < typename U > struct has_peer_id<U, std::void_t<decltype(std::declval<U>().get()->get_peer_id())> > : std::true_type{} ; // 引数が有効な型を表す場合はtrue_typeを継承
  /*
   - std::void_t : 引数が有効な型を表す場合にのみvoid型を生成, decltype(std::declval<T>().get_peer_id())が有効な値を取得できたか否か
   - std::declval<T>() : T型のオブジェクトを"仮"に生成
  */

  template < typename U = T >
  static std::enable_if_t<has_peer_id<U>::value, ss::peer::id >
  get_peer_id( const std::shared_ptr<U>& obj ){
	return obj->get_peer_id();
  } // enable_if_t: 条件が満たされた場合にのみテンプレート関数を有効にする
	// enable_if_t<>の<>の中に判別する型を入れる, 戻り値は書かず enable_ifに制御を任せる(第2引数で指定も可能), _tはエイリアス

  template < typename U = T >
  static std::enable_if_t<!has_peer_id<U>::value, ss::peer::id >
  get_peer_id( const std::shared_ptr<U>& ){
	// return static_cast<std::size_t>(0);
	return ss::peer::id::none();
  }

  static auto get_peer_id_s( typename ss::observer<T>::ref obs_ref ) 
	-> decltype(get_peer_id(obs_ref->get()))
  {
	 return get_peer_id(obs_ref->get()); // observer本体を取り出して渡す 
	 // return boost::uuids::random_generator()();
  }
  
  typedef boost::multi_index::indexed_by
	<

	  boost::multi_index::hashed_unique< boost::multi_index::tag<ss::by_observer_id>
	  , boost::multi_index::const_mem_fun< ss::observer<T>, typename ss::observer<T>::id, &ss::observer<T>::get_id>
	  , typename ss::observer<T>::Hash
	  , typename ss::observer<T>::Equal 
	  > // observer_id

	  , 

	  boost::multi_index::hashed_non_unique< boost::multi_index::tag<by_peer_id> 
	  , boost::multi_index::global_fun< typename ss::observer<T>::ref, ss::peer::id, &CHAIN_SYNC_OBSERVER_STRAGE_POLICY<T>::get_peer_id_s >
	  // const_mem_fun: クラスのメンバ関数を使ってインデックスのキーを取得する
	    // - 第1引数: メンバ関数が属するクラスの型(インデックスのキーとなるオブジェクトの型)
	    // - 第2引数: メンバ関数の戻り値の型(インデックスのキーの型)
		// - 第3引数: 使用するメンバ関数そのもの(インデックスとして使用する関数へのポインタ)
		>
	> index;
};


class chain_sync_manager 
{
  friend class getdata_observer;

public:
  using ref = std::shared_ptr<class chain_sync_manager>;
  using on_finded_forkpoint_handler = std::function<void(void)>; // ローカルチェーンとの分岐点が見つかった時のハンドラ
  using on_complete_handler = std::function<void(void)>; // 仮想チェーンの同期が完了した際のハンドラ
  using on_abort_handler = std::function<void(void)>; // 仮想チェーンの同期が失敗した際のハンドラ

  struct sync_result
  {
	public:
    enum status{
	  SYNC_COMPLETE
		, WRONG_FORKPOINT
		, SYNC_ERROR
		, CONNECTION_ERROR
		, NONE

	  // 正常に同期が終了した
	  // 同期に失敗した
	  // フォークポイントが間違っている
	  // ネットワークエラー(peerとの接続ができない)
    };

    const status state = status::NONE;
    std::vector< block::ref > synced_chain;
	// block_iterator forkpoint;
  
	sync_result( status s, std::vector<block::ref> sc = std::vector<block::ref>() ) : state(s), synced_chain(sc){};
  };

  using on_sync_done_callback = std::function<void(sync_result)>; // 同期の可否にかかわらずchain_managerへの通知用のコールバックを利用
  chain_sync_manager( io_context &io_ctx, block_iterator &forkpoint_itr, on_sync_done_callback notify_func );
  // IBDなどで,chain_sync_managerに先駆けて最新のブロックまでのheader(block_id)を持っている場合は提供する

    // 同期開始系メソッド 
  virtual void async_start( std::pair< block::ref/*未知の最先端ブロック*/, ss::peer::ref/*ブロック受信元peer*/> target ) = 0;
  // 未知のブロックを発見した時の同期シーケンス開始
  // virtual void async_start( std::function<void(sync_result)> ret_callback_func, bool is_use_chain_frame = false ) = 0;
  // virtual sync_result sync_start() = 0; // 成功の可否が分かるまでブロッキングする
  on_sync_done_callback _notify_func;


  bool init( ss::peer::ref peer_ref, const block_id &block_id/*目標(最先端)のblock_ID*/ );
  bool init( std::function<void()> on_failure_confirm_fork_point );

  void income_command_inv( ss::peer::ref peer, inv::ref invs_ref ); // (処理対象) : getblocks_observer, mempool_observer
  void income_command_block( ss::peer::ref peer, MiyaCoreMSG_BLOCK::ref cmd ); // (処理対象) : getdata_observer
  void income_command_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref cmd );

  const bool find_block( const block_id &block_id ) const; // 現在構築している同期用のチェーンの中から該当のブロックを検索する
  bool validate_forkpoint( block_iterator &forkpoint_itr ) const; // 指定したforkpointが有効なものかチェック

  // observerにハンドラとして渡す
  virtual void get_valid_block( block::ref &block_ref ) = 0;

  void send_command_getblocks();
  void send_command_getdata( ss::observer<getdata_observer>::ref getdata_obs ); 
  // getdata_obsのコンテキストに基づいて, getdataの送信と, レスポンス用のobserverをセットアップする
  void register_command_getdata( MiyaCoreMSG_GETDATA &getdata_cmd ); // 自動的にgetdataの展開(block_observerへの変換)とobserver_strageへの追加を行う
  
  void request_prev_block( const block::id &block_id );

  // std::function<void(void)> _on_complete_func = nullptr; // 同期が成功した
  // std::function<void(void)> _on_abort_func = nullptr; // 同期を中断する

  void on_getblocks_done( getblocks_observer::obs_ctx ctx );
   
protected:
  template < typename T > std::vector< typename ss::observer<T>::ref > expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd ); // Tに合致するobserverを抽出する

  io_context &_io_ctx;
  mutable boost::recursive_mutex _rmtx;

  const bool find_forkpoint(); // グローバルチェーンと同期するためにローカルチェーンの分岐点を探す
  
  using chain_sync_observer_strage = ss::observer_strage< CHAIN_SYNC_OBSERVER_STRAGE_POLICY, getdata_observer, block_observer, getheader_observer, getblocks_observer >;
  chain_sync_observer_strage _obs_strage __attribute__((guarded_by(_rmtx)));

  std::vector< std::pair< const block_id, block::ref > > _candidate_synced_chain __attribute__((guarded_by(_rmtx))); // 同期中の仮想チェーン
  using chain_frame = std::vector< std::pair<block_id, bool> >;
  chain_frame _chain_frame; 
  chain_frame::iterator get_undownloaded_blkid_head();


  // このblock_is_vectorに従ってブロックを収集していく　
  // chain_frameの最後尾までブロックが取得できたらチェーンの同期作業を終了する
  // chain_frameの末尾をどうやって定めるか

  int allow_failure_per_block = 5;
  int allow_failure_total = 8;
  
  block_iterator &_forkpoint; 
  bool is_forkpoint_validated = false;
};

class serial_chain_sync_manager : public chain_sync_manager
  // 基本的にローカルチェーンとのfork_pointを見つけ次第,順方向(古->新)にブロック本体のダウンロードを開始する
{
public:
  using ref = std::shared_ptr<serial_chain_sync_manager>;

  serial_chain_sync_manager( io_context &io_ctx, block_iterator fork_point, block::ref target_block /*このブロックまでのチェーンを構築*/ );
  bool init( ss::peer::ref peer_ref, const block_id &block_id/*目標(最先端)のblock_ID*/ );

  void async_start( std::pair< block::ref, ss::peer::ref > target /*同期最終目標, invで受信したもの*/ );
  void start_block_download_sequence();

};


template < typename T > std::vector< typename ss::observer<T>::ref > chain_sync_manager::expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd )
{
  // std::vector< typename ss::observer<T>::ref > ret;
  // return ret;
}


};


#endif 
