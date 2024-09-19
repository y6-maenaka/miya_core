#ifndef AF617A30_B298_41BE_8904_BC1CE6F1D871
#define AF617A30_B298_41BE_8904_BC1CE6F1D871


#include <chain/chain_sync_observer.hpp>
#include <chain/block/block.params.hpp>
// #include <chain/block/block_header.hpp>
#include <chain/chain.hpp>
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
#include "boost/multi_index/mem_fun.hpp"


#include <functional>

using namespace boost::asio;


namespace chain
{


class block;

struct by_peer_id;
struct by_content_id;


template <typename T> struct CHAIN_SYNC_OBSERVER_STRAGE_POLICY
{
  typedef boost::multi_index::indexed_by
	<
	  boost::multi_index::hashed_unique< boost::multi_index::tag<ss::by_observer_id>
	  , boost::multi_index::identity <  typename ss::observer<T>::ref >  // identity:インデックスに使用するキーを指定(基本的にオブジェクトの特定のメンバなどが使われるが, identityは直接オブジェクト自体を直接指定)
	  , typename ss::observer<T>::Hash, typename ss::observer<T>::Equal // 第3引数: hashed_uniqueが使用するハッシュ関数, 第4引数: hashed_uniqueが使用する比較関数を指定
	  > 

	  /* 
	  , 
	  boost::multi_index::hashed_non_unique< boost::multi_index::tag<by_peer_id> 
	  , boost::multi_index::const_global_fun< typename ss::observer<T>::ref, std::size_t, get_peer_id >
	  // const_mem_fun: クラスのメンバ関数を使ってインデックスのキーを取得する
	    // - 第1引数: メンバ関数が属するクラスの型
	    // - 第2引数: メンバ関数の戻り値の型
		// - 第3引数: 使用するメンバ関数そのもの
	  >
	  */
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

  chain_sync_manager( io_context &io_ctx, block_iterator &fork_point_itr );
  chain_sync_manager( io_context &io_ctx, block_iterator &fork_point_itr, std::vector<block_header::ref> chain_frame );
  // IBDなどで,chain_sync_managerに先駆けて最新のブロックまでのheader(block_id)を持っている場合は提供する

  bool init( ss::peer::ref peer_ref, const BLOCK_ID &block_id/*目標(最先端)のblock_ID*/ );
  bool init( std::function<void()> on_failure_confirm_fork_point );
  /* (初期化でやること]
    - fork_pointの確定 -> できなかったらchain_managerに通知して,fork_pointを下げてもらうように依頼する
  */

  void income_command_invs( ss::peer::ref peer, inv::ref invs_ref ); // (処理対象) : getblocks_observer, mempool_observer
  void income_command_block( ss::peer::ref peer, MiyaCoreMSG_BLOCK::ref cmd ); // (処理対象) : getdata_observer
  void income_command_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref cmd );

  const bool find_block( const BLOCK_ID &block_id ) const; // 現在構築している同期用のチェーンの中から該当のブロックを検索する

  // observerにハンドラとして渡す
  virtual void get_valid_block( block::ref &block_ref ) = 0;

  void send_command_getblocks();
  void send_command_getdata( ss::observer<getdata_observer>::ref getdata_obs ); 
  // getdata_obsのコンテキストに基づいて, getdataの送信と, レスポンス用のobserverをセットアップする
  void register_command_getdata( const MiyaCoreMSG_GETDATA &getdata_cmd ); // 自動的にgetdataの展開(block_observerへの変換)とobserver_strageへの追加を行う
  
  void request_prev_block( const block::id &block_id );

  // std::function<void(void)> _on_complete_func = nullptr; // 同期が成功した
  // std::function<void(void)> _on_abort_func = nullptr; // 同期を中断する

  struct sync_result
  {
	public:
    enum status{
      COMPLETE
      , FAILURE_NOT_CONFIRM_BRANCH_POINT
      , ABORT

	  // 正常に同期が終了した
	  // 同期に失敗した
	  // フォークポイントが間違っている
	  // ネットワークエラー(peerとの接続ができない)
    };

    const status state;
    std::vector< block::ref > synced_chain;
  };
  virtual void async_start( std::function<void(sync_result)> ret_callback_func ) = 0;
  virtual sync_result sync_start() = 0; // 成功の可否が分かるまでブロッキングする


protected:
  template < typename T > std::vector< typename ss::observer<T>::ref > expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd ); // Tに合致するobserverを抽出する

  io_context &_io_ctx;
  mutable boost::recursive_mutex _rmtx;

  const bool find_forkpoint(); // グローバルチェーンと同期するためにローカルチェーンの分岐点を探す
  
  using chain_sync_observer_strage = ss::observer_strage< CHAIN_SYNC_OBSERVER_STRAGE_POLICY, getdata_observer, block_observer, getheader_observer >;
  chain_sync_observer_strage _obs_strage __attribute__((guarded_by(_rmtx)));

  std::vector< std::pair< const block_id, block::ref > > _candidate_synced_chain __attribute__((guarded_by(_rmtx))); // 同期中の仮想チェーン
  std::vector< std::pair<block_id, bool/*is_downloaded*/> > _chain_frame; 
  // このblock_is_vectorに従ってブロックを収集していく　
  // chain_frameの最後尾までブロックが取得できたらチェーンの同期作業を終了する
  // chain_frameの末尾をどうやって定めるか

  int allow_failure_per_block = 5;
  int allow_failure_total = 8;
  block_iterator &_block_itr;
};

class serial_chain_sync_manager : public chain_sync_manager
  // 基本的にローカルチェーンとのfork_pointを見つけ次第,順方向(古->新)にブロック本体のダウンロードを開始する
{
public:
  using ref = std::shared_ptr<serial_chain_sync_manager>;

  serial_chain_sync_manager( io_context &io_ctx, block_iterator fork_point, block::ref target_block /*このブロックまでのチェーンを構築*/ );
  serial_chain_sync_manager( io_context &io_ctx, block_iterator fork_point, std::vector<block_header::ref> target_headers /*このヘッダーベクターを元にチェーンを構築*/ );
  bool init( ss::peer::ref peer_ref, const BLOCK_ID &block_id/*目標(最先端)のblock_ID*/ );

  virtual void async_start( std::function<void(sync_result)> ret_callback_func ) = 0;
};


template < typename T > std::vector< typename ss::observer<T>::ref > chain_sync_manager::expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd )
{
  // std::vector< typename ss::observer<T>::ref > ret;
  // return ret;
}

};

#endif 
