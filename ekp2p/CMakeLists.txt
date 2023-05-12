cmake_minimum_required( VERSION 3.2 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )

add_library( EKP2P STATIC 

						 kademlia/common.cpp
						 kademlia/k_bucket.cpp
						 kademlia/k_routing_table.cpp
						 kademlia/k_tag.cpp
						 kademlia/node.cpp

						 kademlia/k_protocol_handler/FIND_NODE.cpp
						 kademlia/k_protocol_handler/PING.cpp
						 kademlia/k_protocol_handler/PONG.cpp


						 network/inband/inband_manager.cpp
						 network/inband/receive_handler.cpp
						 network/inband/udp_inband_manager.cpp

						 network/message/header.cpp
						 network/message/message.cpp

						 network/nat/request_handler.cpp
						 network/nat/response_handler.cpp

						 network/outband/node_outband.cpp

						 network/socket_manager/socket_manager.cpp

)



add_definitions( -std=c++14 )

