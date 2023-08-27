cmake_minimum_required( VERSION 3.2 )


find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)



if( OPENSSL_FOUND )
	include_directories( ${OPENSSL_INCLUDE_DIRS} )
	message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
	# Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()





add_library( SHARED_COMPONENTS SHARED

							cipher/ecdsa_manager.cpp

							hash/sha_hash.cpp

							middle_buffer/middle_buffer.cpp
							smart_middle_buffer/smart_middle_buffer.cpp

)



target_link_libraries( SHARED_COMPONENTS PUBLIC
												${OPENSSL_LIBRARIES}
)





add_definitions( -std=c++14 )
