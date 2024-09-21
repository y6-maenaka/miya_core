#ifndef D176E0FF_F226_40DD_8548_B710C9A9B1BE
#define D176E0FF_F226_40DD_8548_B710C9A9B1BE


// #include <chain/miya_coin/local_strage_manager.h>
// #include <chain/block/block.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <functional>
#include <optional>
#include <string>
#include <array>

#include <chain/miya_coin/local_strage_manager.h>
#include <chain/block/block.params.hpp>
#include "./miya_coin/local_strage_manager.h"
#include <chain/block/block.hpp>


namespace chain
{


class block_iterator //block_iterator と block本体の違いは
{
public:
  block_iterator & operator--();
  block_iterator & operator--(int);

  bool operator ==( const block_iterator &itr );
  block_iterator& operator*();

  std::size_t get_height() const; // _bodyのheightの関節呼び出し
  
  bool is_valid() const;
  static bool (invalid)( const block_iterator &bi );

  block_iterator( BlockLocalStrage::read_block_func read_block_f );

private:
  BlockLocalStrage::read_block_func _read_block_f;
  std::shared_ptr<struct block> _body;
  bool _is_valid;
};


class local_chain // 基本的にチェーン先頭のブロックだけを持つ
{
  struct chain_head 
  {
	struct head_block
	{
	  unsigned char _hash[32];
	  std::uint32_t _height; // 保存の必要ある？
	  std::uint32_t _timestamp;
	  unsigned char _version[4];
	} _head_block __attribute__((packed));

	struct local_synced_file_io; // 先頭が更新んされると,ローカルの記録領域も変更する
  } _chain_head;

public:
  class BlockLocalStrage &_block_strage;
  class block_iterator get_latest_chain_iterator(); // 最先端のチェーンイテレータを取得
  block_iterator (iterator)();
  bool find_block( const block_id &block_id, unsigned int depth = 6 ) const;

public:
  local_chain( std::string path_to_l_chainstate_st/* path to local chainstate st*/, class BlockLocalStrage &block_strage );
  void init();
};

struct local_chain::chain_head::local_synced_file_io
{
  // void write( std::vector<std::uint8_t> );
  // std::vector<std::uint8_t> read( std::size_t bytes );
};


};


#endif 
