#include <mining_manager/mining_manager.hpp>


namespace mining
{


/* mining_manager::mining_control_block::mining_control_block( io_context &io_ctx, struct block_header block_header ) : 
  _id( random_generator()() )
  , _exit_flag(false)
  , _block_header(block_header)
{
  return;
} */

bool mining_manager::mining_control_block::invalid( const mb_id &id )
{
  boost::uuids::uuid invalid_id; 
  // std::fill( invalid_id, '0xff' );
  return id == invalid_id;
}


mining_manager::mining_manager( io_context &io_ctx ) : 
  _io_ctx( io_ctx )
{
  return;
}

/* mining_manager::mb_id mining_manager::mine_block( struct chain::block_header target, std::function<void(void)> on_success_mine, std::function<void(void)> on_failure_mine, unsigned short launch_thread_count, std::time_t timeout )
{
  struct mcb new_mcb;
  for( int i=0; i<launch_thread_count; i++ ){
	struct mining_manager::mining_control_block::segment sgm;
	boost::thread mining_th( &mining_manager::mine_thread_func, std::ref( sgm ) );
	new_mcb._segment.push_back( std::make_pair() );
  }

  if( auto ret_itr = _active_mcb_set.insert( new_mcb ); ret_itr == _active_mcb_set.end() ){
	return invalid_mb_id;
  }
  else{
	return mcb.id;
  }
} */

void mining_manager::stop_mine( const mining_manager::mb_id &id )
{
  return;
}

/* void mining_manager::mine_thread_func( struct block_header header, struct mining_manager::mining_control_block::segment &sgm )
{
  while( true ){
	std::cout << "now mining ..." << "\n";
  }
  return; 
} */


};
