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

// #include <chain/transaction/tx.hpp>
// #include <chain/transaction/tx.utils.hpp>
#include <chain/transaction/p2pkh/p2pkh.h>
#include <chain/transaction_pool/mempool_entry.hpp>


using namespace boost::asio;


namespace chain
{


struct mempool_entry_tx_id 
{
  typedef tx::tx_id result_type; // タグの指定に必要
  result_type operator()( const mempool_entry &entry ) const{
	// return entry.get_tx().get_id();
  } // multi_index_containerで使用するには,さらにハッシュ関数を指定する必要がある
};

class custom_compare_tx_fee
{
public:
  bool operator()( const mempool_entry &entry_a, const mempool_entry &entry_b ) const
  {
	return false;
  }
};

class custom_compare_entryed_at
{
public:
  bool operator()( const mempool_entry &entry_a, const mempool_entry &entry_b ) const
  {
	return false;
  }
};

class custom_compare_entry_bytes
{
public:
  bool operator()( const mempool_entry &entry_a, const mempool_entry &entry_b ) const
  {
	return false;
  }
};

class tx_linear_hasher
{
public:
  std::size_t operator()( const mempool_entry_tx_id::result_type &input ) const
  {
	return 10;
  }
};

// 検索用の名前(タグ定義)
struct index_by_tx_id;
struct index_by_tx_fee;
struct index_by_entry_bytes;
struct index_by_entryed_at;


class mempool
{
public:
  mempool( io_context &io_ctx );


  typedef boost::multi_index_container<
	class mempool_entry
	, boost::multi_index::indexed_by<
		boost::multi_index::hashed_unique< mempool_entry_tx_id , tx_linear_hasher> // indexed by tx_id
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_tx_fee>, boost::multi_index::identity<mempool_entry>, custom_compare_tx_fee > // indexed by fee
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_entryed_at>, boost::multi_index::identity<mempool_entry>, custom_compare_entryed_at > // indexed by entryed_at
		, boost::multi_index::ordered_non_unique< boost::multi_index::tag<index_by_entry_bytes>, boost::multi_index::identity<mempool_entry>, custom_compare_entry_bytes > // indexed by entry bytes
	  >	
	> indexed_tx_set;

	// multi_index::tag<> 複数インデックスが付与されているときに識別する (一つだけ指定しないとそれがデフォになる)
	/*
	 - ordered_unique(順序を保持し,重複を許さない) = (set
	 - ordered_non_unique(順序を保持し,重複を許す) = (multi_set
	 - hashed_unique(ハッシュテーブルによって要素を格納し,重複を許さない) = (unordered_set
	 - hashed_non_unique(ハッシュテーブルによって要素を格納し,重複を許す) = (unordered_multi_set

	 // tag : 識別子
	 // identity : 何に基づいてインデックスされるのか(以降の関数を)
	 // sequenced : 素運輸潤を記憶する(list的な)
	*/
  
  indexed_tx_set::iterator add_entry( const mempool_entry &entry );
private:
  deadline_timer _refresh_timer;
  mutable boost::recursive_mutex _rmtx;
  indexed_tx_set _indexed_tx_set __attribute__((guarded_by(_rmtx)));

  std::size_t _total_bytes __attribute__((guarded_by(_rmtx)));
  long long _total_fee __attribute__((guarded_by(_rmtx)));
};


};


#endif 
