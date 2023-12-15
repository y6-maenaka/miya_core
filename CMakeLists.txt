cmake_minimum_required( VERSION 3.12 )

project(MIYA_CORE CXX)
add_subdirectory( miya_core )
add_subdirectory( ekp2p )
add_subdirectory( miya_chain )
add_subdirectory( miya_db )
add_subdirectory( control_interface )
add_subdirectory( shared_components )



include_directories( ${OPENSSL_INCLUDE_DIRS} )
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )




if( BUILD_DB )
add_executable( db_ver_0 ./release/db/ver_0.cpp )
target_link_libraries( db_ver_0 PRIVATE
												MIYA_CORE
												MIYA_DB
												SHARED_COMPONENTS
												MIYA_CHAIN
)
target_compile_options( db_ver_0 PUBLIC -Wall )
endif()





if( BUILD_CORE )
add_executable( core_ver_0 ./release/core/ver_0.cpp )
target_link_libraries( core_ver_0 PRIVATE
												MIYA_CORE
												MIYA_DB
												SHARED_COMPONENTS
												EKP2P
												MIYA_CHAIN
												CONTROL_INTERFACE
)
#target_include_directories( core_ver_0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_compile_options( core_ver_0 PUBLIC -Wall )
endif()





if( BUILD_EKP2P_TEST )
add_executable( node_0 ./release/ekp2p_test/node_0.cpp )
target_link_libraries( node_0 PRIVATE
												MIYA_CORE
												MIYA_DB
												EKP2P
												MIYA_CHAIN
												CONTROL_INTERFACE
												SHARED_COMPONENTS
)
target_compile_options( node_0 PUBLIC -Wall )

add_executable( node_1 ./release/ekp2p_test/node_1.cpp )
target_link_libraries( node_1 PRIVATE
												MIYA_CORE
												MIYA_DB
												EKP2P
												MIYA_CHAIN
												CONTROL_INTERFACE
												SHARED_COMPONENTS
)
target_compile_options( node_1 PUBLIC -Wall )

endif()







add_definitions( -std=c++20 )
