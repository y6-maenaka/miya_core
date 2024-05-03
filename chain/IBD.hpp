#ifndef B91C1050_1882_4050_906E_9C3D404CBF86
#define B91C1050_1882_4050_906E_9C3D404CBF86

#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <cassert>
#include <chrono>

#include <node_gateway/message/message.hpp>
#include <node_gateway/message/command/command_set.hpp>
#include <chain/chain_manager.h>


namespace chain
{


class IBD
{
public:
  IBD( class chain_manager &chain_manager );

  bool start(); // 終了するまでブロッキングする
private:
  class chain_manager &_chain_manager;
};


};



#endif // B91C1050_1882_4050_906E_9C3D404CBF86
