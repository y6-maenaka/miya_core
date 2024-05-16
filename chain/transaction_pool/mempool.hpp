#ifndef E1C5FA98_3C3D_471A_8238_67A8A2408974
#define E1C5FA98_3C3D_471A_8238_67A8A2408974


#include "boost/asio.hpp"
#include "boost/thread.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/tag.hpp"
#include "boost/multi_index/identity.hpp"
#include "boost/multi_index/indexed_by.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/sequenced_index.hpp"

#include <mutex>
#include <string>
#include <optional>

// #include <chain/transaction/tx.hpp>
// #include <chain/transaction/tx.utils.hpp>
// #include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/transaction_pool/mempool_entry.hpp>


using namespace boost::asio;


namespace chain
{


constexpr std::int64_t ALLOWED_MAX_POOL_SIZE_BYTES = 300 * 1024 * 1024; // 最大300MBまで
constexpr std::chrono::seconds ALLOWED_TX_EXPIRY = std::chrono::seconds(2 * 7 * 24 * 60 * 60); // 最大2weeksまで
constexpr std::int64_t ALLOWED_MAX_TX_SIZE_BYTES = 100 * 1024; // txの最大サイズは100KB


struct mempool_entry_tx_id 
{
  typedef tx::tx_id result_type; // タグの指定に必要
  result_type operator()( const mempool_entry &entry ) const{
	return entry.get_tx_id();
  } // multi_index_containerで使用するには,さらにハッシュ関数を指定する必要がある
};

class mempool_entry_compare_tx_fee
{
public:
  bool operator()( const mempool_entry &entry_1, const mempool_entry &entry_2 ) const
  {
	return entry_1.get_tx_fee() < entry_2.get_tx_fee();
  }
};

class mempool_entry_compare_entryed_at
{
public:
  bool operator()( const mempool_entry &entry_1, const mempool_entry &entry_2 ) const
  {
	return entry_1.get_entryed_at() < entry_2.get_entryed_at();
  }
};

class mempool_entry_compare_entry_size_bytes
{
public:
  bool operator()( const mempool_entry &entry_1, const mempool_entry &entry_2 ) const
  {
	return entry_1.get_entry_size_bytes() < entry_2.get_entry_size_bytes();
  }
};

class tx_id_linear_hasher 
{
public:
  std::size_t operator()( const mempool_entry_tx_id::result_type &input ) const
  {
	std::string input_str( reinterpret_cast<const char*>(input.data(), input.size() ) );
	return std::hash<std::string>()(input_str);
  }
};


// 検索用の名前(タグ定義)
struct index_by_tx_id;
struct index_by_tx_fee;
struct index_by_entry_bytes;
struct index_by_entryed_at;


class mempool
{
private:
  io_context &_io_ctx; // 基本的にリフレッシュのみに使用する
  deadline_timer _refresh_timer;
  mutable boost::recursive_mutex _rmtx; // for const関数内からの使用

public:
  mempool( io_context &io_ctx );

  typedef boost::multi_index_container<
	class mempool_entry
	, boost::multi_index::indexed_by<
		boost::multi_index::hashed_unique< mempool_entry_tx_id , tx_id_linear_hasher> // indexed by tx_id
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_tx_fee>, boost::multi_index::identity<mempool_entry>, mempool_entry_compare_tx_fee > // indexed by fee
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_entryed_at>, boost::multi_index::identity<mempool_entry>, mempool_entry_compare_entryed_at > // indexed by entryed_at
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_entry_bytes>, boost::multi_index::identity<mempool_entry>, mempool_entry_compare_entry_size_bytes > // indexed by entry bytes
	  >	
	> indexed_tx_set;

	// boost::multi_index_container Guide
	// multi_index::tag<>: getなどのラベル名になる 複数インデックスが付与されているときに識別する (一つだけ指定しないとそれがデフォになる?)
	// multi_index::identity: 要素そのものがindex対象になる第3引数にカスタム比較関数などをしている場合, identityで指定した要素をその関数の入力とする
	/*
	 - ordered_unique(順序を保持し,重複を許さない) = (set
	 - ordered_non_unique(順序を保持し,重複を許す) = (multi_set
	 - hashed_unique(ハッシュテーブルによって要素を格納し,重複を許さない) = (unordered_set
	 - hashed_non_unique(ハッシュテーブルによって要素を格納し,重複を許す) = (unordered_multi_set

	 // tag : 識別子
	 // identity : 何に基づいてインデックスされるのか(以降の関数を)
	 // sequenced : 素運輸潤を記憶する(list的な)
	*/
  
  indexed_tx_set::iterator add_entry( const mempool_entry &entry ); // 単純トランザクション追加メソッド
  void remove_entry( const struct tx::P2PKH &target ); // 単純トランザクション削除メソッド
  void remove_for_reorg( std::vector<tx::transaction_ref> txs ); // チェーンの再編集(アクシデントフォークなど)が必要になった際にトランザクションを削除する
  void remove_for_block( std::vector<tx::transaction_ref> txs, unsigned int block_height ); // ブロックがチェーンに繋がれた時に,ブロックに含まれているトランザクションを削除する

  const mempool_entry get_entry( const tx::tx_id &txid ); // tx_idからmempool_entryを取得する
  std::optional< indexed_tx_set::iterator > get_txitr( const tx::tx_id &txid ); // tx_idからmempoolのtxイテレータを取得する

  std::size_t get_tx_count() const; // mempoolに保存されているトランザクション個数を取得する

  indexed_tx_set _indexed_tx_set __attribute__((guarded_by(_rmtx))); // block assembler 等は直接これにアクセスして取得する
  std::vector< tx::transaction_ref > _txs_randomized __attribute__((__guarded_by__(_rmtx))); // bitcoin coreで実装されている(本プログラムでは今の所使用しない)

  using txitr = indexed_tx_set::iterator;

  const std::int64_t allowed_max_pool_size_bytes{ALLOWED_MAX_POOL_SIZE_BYTES}; // mempoolの最大サイズ
  const std::chrono::seconds allowed_tx_expiry{ALLOWED_TX_EXPIRY}; // トランザクションの保存有効期限
  const std::int64_t allowed_max_tx_size_bytes{ALLOWED_MAX_TX_SIZE_BYTES}; // mempoolに入れるトランザクションの最大サイズ

protected:
  std::size_t _total_mem_usage_bytes __attribute__((guarded_by(_rmtx))){0};  // poolにストアされているentryの総バイト数
  std::size_t _total_tx_size_bytes __attribute__((guarded_by(_rmtx))){0}; // poolにストアされているtxの総バイト数
  std::size_t _total_fee __attribute__((guarded_by(_rmtx))){0}; // pool内トランザクションの合計手数料
};


};


#endif 
