cmake_minimum_required( VERSION 3.12 )

 
find_package(PkgConfig REQUIRED) # 外部パッケージを検出する準備(opensslは外部パッケージ(?))
# pkg_check_modules(OPENSSL REQUIRED openssl)
# pkg_search_module(OPENSSL REQUIRED openssl)
find_package( OPENSSL REQUIRED )


if( OPENSSL_FOUND )
	include_directories( ${OPENSSL_INCLUDE_DIRS} )
	message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
	message(STATUS "Openssl IncludeDir :: ${OPENSSL_INCLUDE_DIRS}")
else()
	# Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()




add_library( SHARED_COMPONENTS SHARED

							cipher/ecdsa_manager.cpp

							hash/sha_hash.cpp

							#middle_buffer/middle_buffer.cpp
							stream_buffer/stream_buffer.cpp
							stream_buffer/stream_buffer_container.cpp
							stream_buffer/test/unit_test.cpp

							miya_db_client/miya_db_client.cpp
							miya_db_client/miya_db_sb_client.cpp
)



target_link_libraries( SHARED_COMPONENTS PUBLIC
												${OPENSSL_LIBRARIES}
												EKP2P #SBSegmentをekp2p::headerから作成するために必要
)



add_definitions( -std=c++20 -w )
