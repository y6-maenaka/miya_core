#ifndef D14A14B9_C58C_495D_ADE2_1CED0783D2A9
#define D14A14B9_C58C_495D_ADE2_1CED0783D2A9


namespace chain
{


class mempool;
class chain_state;


class block_template 
{ 
  /*
  block;
  block_fee;
  その他のメタ情報
  */
};

class block_assembler
{
  /*
  mempool
  chain_state
  options.min_fee 等
  */
public: 
  block_assembler( const mempool *mpool ); // トランザクション参照の為

  block_template create_new_block( chain_state &chain_state, const mempool *mpool ); 
};


};


#endif 
