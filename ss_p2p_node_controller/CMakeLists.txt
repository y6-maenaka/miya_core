cmake_minimum_required( VERSION 3.12 )


find_package( PkgConfig REQUIRED ) # 外部パッケージを検出する準備(opensslは外部パッケージ(?))
find_package( OpenSSL REQUIRED )
find_package( Boost REQUIRED )

# .hpp, .cppをまとめる
set( basic_include_files
	  include/json.hpp
	  include/hash.hpp
	  include/utils.hpp
)
set( basic_source_files 
	  src/utils.cpp
)
set( ss_p2p_include_files
    node_controller.hpp
	peer.hpp
	endpoint.hpp
	message.hpp
	message_pool.hpp
	socket_manager.hpp
	udp_server.hpp
	observer.hpp
	observer_strage.hpp
	sender.hpp
)
set( ss_p2p_source_files
    node_controller.cpp
	peer.cpp
	endpoint.cpp
	message.cpp
	message_pool.cpp
	socket_manager.cpp
	udp_server.cpp
	observer.cpp
	observer_strage.cpp
	sender.cpp
)
set( ss_p2p_kademlia_include_files
    k_routing_table.hpp
    k_bucket.hpp
    k_node.hpp
	dht_manager.hpp
	node_id.hpp
	k_observer.hpp
	k_observer_strage.hpp
	k_message.hpp
	rpc_manager.hpp
	direct_routing_table_controller.hpp
)
set( ss_p2p_kademlia_source_files
    k_routing_table.cpp
    k_bucket.cpp
    k_node.cpp
	dht_manager.cpp
	node_id.cpp
	k_observer.cpp
	k_observer_strage.cpp
	k_message.cpp
	rpc_manager.cpp
	direct_routing_table_controller.cpp
	connection_maintainer.cpp
)
set( ss_p2p_ice_agent_include_files 
	ice_agent.hpp
	ice_observer.hpp
	ice_observer_strage.hpp
	signaling_server.hpp
	stun_server.hpp
	ice_message.hpp
	ice_sender.hpp
)
set( ss_p2p_ice_agent_source_files 
	ice_agent.cpp
	ice_observer.cpp
	ice_observer_strage.cpp
	signaling_server.cpp
	stun_server.cpp
	ice_message.cpp
	ice_sender.cpp
)

# リスト要素にプレフィックスを付与
list(TRANSFORM ss_p2p_include_files PREPEND "include/ss_p2p/" )
list(TRANSFORM ss_p2p_source_files PREPEND "src/ss_p2p/")
list(TRANSFORM ss_p2p_kademlia_include_files PREPEND "include/ss_p2p/kademlia/")
list(TRANSFORM ss_p2p_kademlia_source_files PREPEND "src/ss_p2p/kademlia/")
list(TRANSFORM ss_p2p_ice_agent_include_files PREPEND "include/ss_p2p/ice_agent/")
list(TRANSFORM ss_p2p_ice_agent_source_files PREPEND "src/ss_p2p/ice_agent/")

add_library( SS_P2P STATIC # ss_p2pをライブラリとしてまとめる
			${basic_include_files}
			${basic_source_files}
            ${ss_p2p_include_files}
            ${ss_p2p_source_files}
)
# 共通のインストールディレクトリとソールディレクトリの指定
target_include_directories( SS_P2P PUBLIC # 一応依存先には公開しない
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

			${OPENSSL_INCLUDE_DIR}
			${Boost_INCLUDE_DIRS}
)

target_link_libraries( SS_P2P PRIVATE
			${OPENSSL_LIBRARIES}
			${Boost_LIBRARIES}
)

# if KADEMLIA FLAG
target_sources( SS_P2P PRIVATE
              ${ss_p2p_kademlia_source_files}
              ${ss_p2p_kademlia_include_files}

)
target_sources( SS_P2P PRIVATE
			  ${ss_p2p_ice_agent_source_files}
			  ${ss_p2p_ice_agent_include_files}
)

add_definitions( -std=c++20 )
