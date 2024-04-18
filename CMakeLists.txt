cmake_minimum_required( VERSION 3.12 )

project(core CXX)

find_package( PkgConfig REQUIRED ) # 外部パッケージを検出する準備(opensslは外部パッケージ(?))
find_package( OpenSSL REQUIRED )
find_package( Boost REQUIRED COMPONENTS program_options )

include_directories(
	${Boost_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/share>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ss_p2p_node_controller/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # 全部こっちでいいかも
)

# add_subdirectory( ekp2p )
add_subdirectory( ss_p2p_node_controller )
add_subdirectory( chain )
add_subdirectory( miya_db )
add_subdirectory( control_interface )
add_subdirectory( node_gateway )
add_subdirectory( share )
add_subdirectory( core )

# add_subdirectory( test/miya_db )
# add_subdirectory( test/ekp2p )
# add_subdirectory( test/chain )

add_dependencies( core SS_P2P )

if( _BUILD_CORE_DEBUG )
add_executable( core_ver_0 ./release/core/ver_0.cpp )
target_link_libraries( core_ver_0 PRIVATE
	core
	MIYA_DB
	CHAIN
	CONTROL_INTERFACE
	node_gateway
	SHARE
	SS_P2P

	# TEST_chain
	Boost::program_options
)

target_compile_options( core_ver_0 PUBLIC -Wall )
# add_dependencies( TEST_chain SS_P2P)
add_dependencies( core_ver_0 SS_P2P)
add_compile_definitions( DEBUG )
add_compile_definitions( VERBOSE )
endif()


add_definitions( -std=c++20 )
