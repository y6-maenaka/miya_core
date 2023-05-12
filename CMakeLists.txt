cmake_minimum_required( VERSION 3.2 )

project(MIYA_CORE CXX)
add_subdirectory( ekp2p )
add_subdirectory( miya_core )
add_subdirectory( miya_db )
add_subdirectory( shared_components )




include_directories( ${OPENSSL_INCLUDE_DIRS} )


if( BUILD_CORE )
add_executable( core_ver_0 ./release/core/ver_0.cpp )
target_link_libraries( core_ver_0 PRIVATE
												EKP2P
												MIYA_CORE
												SHARED_COMPONENTS						
)
target_compile_options( core_ver_0 PUBLIC -Wall )
endif()




if( BUILD_DB )
add_executable( db_ver_0 ./release/db/ver_0.cpp )
target_link_libraries( db_ver_0 PRIVATE
												MIYA_DB
												SHARED_COMPONENTS
)
target_compile_options( db_ver_0 PUBLIC -Wall )
endif()







add_definitions( -std=c++14 )


