#include <chain/chain.hpp>


namespace chain
{


local_chain::local_chain( std::string path_to_l_chainstate_st, class BlockLocalStrage &block_strage ) :
  _block_strage( block_strage )
{
  return;
}


block_iterator::block_iterator( BlockLocalStrage::read_block_func read_block_f ) :
  _read_block_f( read_block_f )
{
  return;
}

block_iterator &block_iterator::operator--()
{
  if( !(is_valid()) ) return *this;
 
  block::block_hash bh = _body->get_prev_block_hash(); // get block hash
  if( auto readed_block = _read_block_f( bh ); readed_block == nullptr ) _is_valid = false;
  else
  {
	_body = readed_block;
	_is_valid = true;
  }
  return *this;
}

block_iterator &block_iterator::operator--(int)
{
  // block_iterator ret = *this; 
}

bool block_iterator::invalid( const block_iterator &bi )
{
  return (!bi._is_valid);
}


};
