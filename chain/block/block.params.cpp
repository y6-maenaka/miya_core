#include <chain/block/block.params.hpp>


namespace chain
{


static block_id block_id::invalid()
{
  block_id ret; ret.fill(0xff);
  return ret;
}

};
