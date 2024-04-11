cmake_minimum_required( VERSION 3.12 )

project(MIYA_CORE CXX)

find_package( PkgConfig REQUIRED ) # 外部パッケージを検出する準備(opensslは外部パッケージ(?))
find_package( OpenSSL REQUIRED )
find_package( Boost REQUIRED )

include_directories(
	${Boost_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ss_p2p_node_controller/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shared_components>
)

# add_subdirectory( ekp2p )
add_subdirectory( ss_p2p_node_controller )
add_subdirectory( miya_chain )
add_subdirectory( miya_db )
add_subdirectory( control_interface )
add_subdirectory( shared_components )
add_subdirectory( miya_core )

# add_subdirectory( test/miya_db )
# add_subdirectory( test/ekp2p )
# add_subdirectory( test/miya_chain )

add_dependencies( MIYA_CORE SS_P2P )

if( _BUILD_CORE )
add_executable( core_ver_0 ./release/core/ver_0.cpp )
target_link_libraries( core_ver_0 PRIVATE
	MIYA_CORE
	MIYA_DB
	MIYA_CHAIN
	CONTROL_INTERFACE
	SHARED_COMPONENTS
	SS_P2P

	# TEST_MIYA_CHAIN
)

target_compile_options( core_ver_0 PUBLIC -Wall )
# add_dependencies( TEST_MIYA_CHAIN SS_P2P)
add_dependencies( core_ver_0 SS_P2P)
endif()


add_definitions( -std=c++20 )
