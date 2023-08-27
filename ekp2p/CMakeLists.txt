cmake_minimum_required( VERSION 3.2 )


include_directories( ${OPENSSL_INCLUDE_DIRS} )


#add_subdirectory( kademlia )
#add_subdirectory( network )


add_library( EKP2P STATIC 

						 ekp2p.cpp

						 kademlia/common.cpp
						 kademlia/k_bucket.cpp
						 kademlia/k_routing_table.cpp
						 kademlia/k_tag.cpp
						 kademlia/node.cpp

						 kademlia/k_node.cpp
						 kademlia/message_receiver.cpp
						 kademlia/connection_controller.cpp
						 network/header/header.cpp
						 


						 kademlia/kademlia_RPC/FIND_NODE.cpp
						 kademlia/kademlia_RPC/PING.cpp
						 kademlia/kademlia_RPC/PONG.cpp
						 
						 kademlia/table_wrapper/table_wrapper.cpp


						 network/inband/inband_manager.cpp
						 network/inband/receive_handler.cpp
						 network/inband/default_message_handler.cpp
						 network/inband/k_tag_pack.cpp

						 network/message/header.cpp
						 network/message/message.cpp

						 network/nat/request_handler.cpp
						 network/nat/response_handler.cpp
						 network/nat/stun_message.cpp
						 network/nat/client_nat_manager.cpp
						 network/nat/stun_message_handler.cpp
						 

						 network/outband/node_outband.cpp
						 network/outband/node_broadcaster.cpp

						 network/socket_manager/socket_manager.cpp

)


add_definitions( -std=c++14 )

