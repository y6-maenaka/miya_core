cmake_minimum_required( VERSION 3.12 )


add_library( node_gateway STATIC

  message/message.cpp
  message/command/inv/inv.cpp
  message/command/block.cpp
  message/command/getblocks.cpp
  message/command/getdata.cpp
  message/command/headers.cpp
  message/command/notfound.cpp

  ss_command_observer.cpp
  ss_command_observer_strage.cpp
)

add_definitions( -std=c++20 -w )
