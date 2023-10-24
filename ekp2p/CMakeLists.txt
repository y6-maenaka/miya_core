cmake_minimum_required( VERSION 3.2 )


include_directories( ${OPENSSL_INCLUDE_DIRS} )

#add_subdirectory( kademlia )
#add_subdirectory( network )


add_library( EKP2P STATIC 

						 ekp2p.cpp

						 kademlia/k_node.cpp
						 
						 kademlia/k_routing_table/k_routing_table.cpp
						 kademlia/k_routing_table/k_bucket.cpp
						 
						 network/header/header.cpp
						 network/message/message.cpp

						 network/nat/request_handler.cpp
						 network/nat/stun_message.cpp
						 network/nat/client_nat_manager.cpp
						 

						 network/socket_manager/socket_manager.cpp

						 daemon/routing_table_manager/routing_table_manager.cpp
						 daemon/routing_table_manager/k_client_node_swap_wait_queue.cpp
						 daemon/routing_table_manager/__test.cpp
						 daemon/sender/sender.cpp
						 daemon/receiver/receiver.cpp
						 daemon/broker/broker.cpp
)


add_definitions( -std=c++17 )

