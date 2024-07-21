#ifndef AF617A30_B298_41BE_8904_BC1CE6F1D871
#define AF617A30_B298_41BE_8904_BC1CE6F1D871


#include <chain/chain_sync_observer.hpp>
#include <chain/block/block.params.hpp>
#include <chain/chain.hpp>
#include <ss_p2p/observer_strage.hpp>
#include <ss_p2p/peer.hpp>
#include <node_gateway/message/command/inv.hpp>
#include <node_gateway/message/command/notfound.hpp>
#include <node_gateway/message/command/block.hpp>

#include "boost/asio.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include <functional>

using namespace boost::asio;


namespace chain
{


class block;


class chain_sync_manager 
{
  friend class getdata_observer;

public:
  using ref = std::shared_ptr<class chain_sync_manager>;
  using on_finded_forkpoint_handler = std::function<void(void)>; // ローカルチェーンとの分岐点が見つかった時のハンドラ
  using on_complete_handler = std::function<void(void)>; // 仮想チェーンの同期が完了した際のハンドラ
  using on_abort_handler = std::function<void(void)>; // 仮想チェーンの同期が失敗した際のハンドラ

  chain_sync_manager( io_context &io_ctx, block_iterator &fork_point_itr );

  bool init( ss::peer::ref peer_ref, const BLOCK_ID &block_id/*目標(最先端)のblock_ID*/ );
  void income_command_invs( ss::peer::ref peer, inv::ref invs_ref );
  void income_command_block( ss::peer::ref peer, MiyaCoreMSG_BLOCK::ref cmd );
  void income_command_notfound( ss::peer::ref peer, MiyaCoreMSG_NOTFOUND::ref cmd );

  const bool find_block( const BLOCK_ID &block_id ) const; // 現在構築している同期用のチェーンの中から該当のブロックを検索する

  // observerにハンドラとして渡す
  virtual void get_valid_block( block::ref &block_ref ) = 0;

  void send_command_getdata( ss::observer<getdata_observer>::ref getdata_obs ); 
  // getdata_obsのコンテキストに基づいて, getdataの送信と, レスポンス用のobserverをセットアップする
  void register_command_getdata( const MiyaCoreMSG_GETDATA &getdata_cmd ); // 自動的にgetdataの展開(block_observerへの変換)とobserver_strageへの追加を行う
  
  void request_prev_block( const block::id &block_id );

  std::function<void(void)> _on_complete_func = nullptr; // 同期が成功した
  std::function<void(void)> _on_abort_func = nullptr; // 同期を中断する

  virtual void async_start() = 0;
  virtual void sync_start() = 0; // 成功の可否が分かるまでブロッキングする

  struct sync_result
  {
    enum status{
      COMPLETE
      , FAILURE_NOT_CONFIRM_BRANCH_POINT
      , ABORT
    };

    const status _state;
    std::vector< block::ref > synced_chain;
  };

protected:
  template < typename T > std::vector< typename ss::observer<T>::ref > expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd ); // Tに合致するobserverを抽出する

  io_context &_io_ctx;
  mutable boost::recursive_mutex _rmtx;

  void find_forkpoint(); // グローバルチェーンと同期するためにローカルチェーンの分岐点を探す
  std::vector< std::pair< const BLOCK_ID, std::shared_ptr<class block>> > _candidate_chain __attribute__((guarded_by(_rmtx))); // 同期中の仮想チェーン
  
  using chain_sync_observer_strage = ss::observer_strage< getdata_observer, block_observer, getheader_observer >;
  chain_sync_observer_strage _obs_strage __attribute__((guarded_by(_rmtx)));

  const BLOCK_ID _start_block_id; 
  const BLOCK_ID _end_block_id;

  int allow_failure_per_block = 5;
  int allow_failure_total = 8;
  block_iterator &_block_itr;
};

class serial_backward_chain_sync_manager : public chain_sync_manager
  // 基本的にローカルチェーンとのfork_pointを見つけ次第,順方向(古->新)にブロック本体のダウンロードを開始する
{
public:
  using ref = std::shared_ptr<serial_backward_chain_sync_manager>;


public:
  void find_forkpoint(); // 逆方向の場合は必要ない

  serial_backward_chain_sync_manager( io_context &io_ctx );
  bool init( const BLOCK_ID &block_id ); 
  // bool init_based_headers( std::vector<block_header::ref> header_v );

  void sync_start();
  void async_start( const on_complete_handler &on_complete_func, const on_abort_handler &on_abort_func );
  void on_get_valid_block( block::ref &block_ref );

private:
  const block::id &_target_block_id;
};


template < typename T > std::vector< typename ss::observer<T>::ref > chain_sync_manager::expand_command_getdata( const MiyaCoreMSG_GETDATA::ref &cmd )
{
  // std::vector< typename ss::observer<T>::ref > ret;
  // return ret;
}

};

#endif 
