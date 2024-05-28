#ifndef D14A14B9_C58C_495D_ADE2_1CED0783D2A9
#define D14A14B9_C58C_495D_ADE2_1CED0783D2A9


#include <memory>


namespace chain
{


class mempool;
class chain_state;


struct block_template 
{ 
  /*
  block;
  block_fee;
  その他のメタ情報
  */
};


// マイニングに必要な(マイニングする)ブロックを適当にメモリープールから取り出す
// create_new_block()内部で新たなcoinbaseのセットアップとブロックへの挿入も行う
class block_assembler
{
  /*
  mempool
  chain_state
  options.min_fee 等
  */

private:
  std::unique_ptr<struct block_template > _block_template; // 現在管理中のブロックテンプレート(=マイニング中)
  unsigned int _height;
  unsigned int _fee; // このブロックをマイニングしたときに入手できる手数料
  const mempool *_mempool;
  chain_state *_chain_state;

protected:
  void reset_block(); 

public: 
  block_assembler( const mempool *mpool ); // トランザクション参照の為

  block_template create_new_block( chain_state &chain_state, const mempool *mpool );  // この中でcoinbaseを作成する
};


};


#endif 
