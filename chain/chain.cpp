#include <chain/chain.hpp>


namespace chain
{


local_chain::local_chain( std::string path_to_l_chainstate_st, BlockLocalStrageManager &block_strage_manager ) :
  _block_strage_manager( block_strage_manager )
{
  return;
}

block_iterator & block_iterator::operator--()
{
  if( !(is_valid()) ) return *this;
  
  // block_hash bh = _body->get_block_hash();
  // auto readed_block = _read_block_f( bh );
}


};
