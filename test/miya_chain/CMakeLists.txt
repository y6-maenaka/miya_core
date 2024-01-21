cmake_minimum_required( VERSION 3.12 )

include_directories( ${OPENSSL_INCLUDE_DIRS} )


add_library( TEST_MIYA_CHAIN STATIC

			  common.cpp
			  virtual_chain_p1.cpp
			  virtual_chain_p2.cpp
				build_sample_chain_p1.cpp
)


add_definitions( -std=c++20 -w )
