#include <node_gateway/ss_command_observer.hpp>


getdata::getdata( io_context &io_ctx ) : 
  base_observer(io_ctx)
{
  return;
}
