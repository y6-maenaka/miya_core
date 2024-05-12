#ifndef A34466E5_284C_405B_865F_CEC134C2384C
#define A34466E5_284C_405B_865F_CEC134C2384C


#include <functional>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <limits>

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/thread.hpp"
#include "boost/unordered_set.hpp"


#include <chain/block/block.hpp>
// #include <chain/block/block_header.hpp>


using namespace boost::asio;


namespace mining
{


class mining_manager
{
public:
  using mb_id = boost::uuids::uuid;// マイニングしているブロック処理プロセス(単位)id(ブロックハッシュではない)

  struct mining_control_block { // マイニング中のプロセス(仮想の単位)を管理するcontrol block
	 struct segment
	 {
	   friend mining_manager::mining_control_block;

	   deadline_timer timeout_timer; // タイムアウトを設定した時用
	   std::uint32_t start_point; // 任意のnonce値でマイニングを開始する
	   std::uint32_t break_point; // 任意のnonce値でマイニングを打ち切る
	   segment( io_context &io_ctx, std::uint32_t start_point = std::numeric_limits<std::uint32_t>::min(), std::uint32_t end_point = std::numeric_limits<std::uint32_t>::max() );
	 };

	 mb_id _id;
	 // struct block_header _block_header;
	 // std::vector< boost::thread, struct segment > _segments;
	 boost::recursive_mutex _rmtx; 
	 bool _exit_flag = {false};

	 mining_control_block( io_context &io_ctx, struct blcok_header block_header );
	 static bool (invalid)( const mb_id &id );
  };
  using mcb = struct mining_control_block;

  std::unordered_set< std::shared_ptr<mcb> > _active_mcb_set; // 基本的にマイニング中のブロックは一つ
 
  mb_id mine_block( struct chain::block target /*マークルルートのシャッフルが必要になる可能性があるためblock本体 ※コピーして渡す*/
	  , std::function<void(void)> on_success_mine/*マイニングに成功した時のコールバック*/
	  , std::function<void(void)> on_failure_mine
	  , unsigned short launch_thread_count = 1, std::time_t timeout = 0 ); 
  void stop_mine( const mb_id &id );

  mining_manager( io_context &io_ctx );

protected:
  void mine_thread_func( struct block_header header, struct mining_manager::mining_control_block::segment &sgm ); // マイニングスレッド関数
 
private:
  io_context &_io_ctx;
};


};


#endif 

