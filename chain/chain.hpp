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


namespace chain
{


struct Block;


class block_iterator //block_iterator と block本体の違いは
{
public:
  std::shared_ptr<struct Block> _body;
  class BlockLocalStrageManager&  _local_strage_manager;

  // block_iterator & operator++(); // 前置
  // block_iterator & operator++(int); // 後置

  block_iterator & operator--();
  block_iterator & operator--(int);

  bool operator ==( const block_iterator &itr );
  block_iterator& operator*();

  std::size_t get_height() const; // _bodyのheightの関節呼び出し
  bool is_valid() const;

private:
  std::function<std::shared_ptr<struct Block>(std::array<std::uint8_t, 256/8>)/*あとで 置き換える*/> _read_block_f;
  bool _is_vaild;
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

  class BlockLocalStrageManager &_block_strage_manager;
  class chain_iterator get_chain_iterator();

public:
  local_chain( std::string path_to_l_chainstate_st, BlockLocalStrageManager &block_strage_manager );
  void init();
};

struct local_chain::chain_head::local_synced_file_io
{
  void write( std::vector<std::uint8_t> );
  std::vector<std::uint8_t> read( std::size_t bytes );
};


};


#endif 
