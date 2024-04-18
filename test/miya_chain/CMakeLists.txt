cmake_minimum_required( VERSION 3.12 )

add_library( TEST_chain SHARED
			  common.cpp
			  virtual_chain_p1.cpp
			  virtual_chain_p2.cpp
			  virtual_chain_p3.cpp
			  virtual_chain_p4.cpp
			  virtual_chain_p5.cpp
			  build_sample_chain_p1.cpp
)


add_definitions( -std=c++20 -w )
