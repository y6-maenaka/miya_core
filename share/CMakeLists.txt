cmake_minimum_required( VERSION 3.12 )


add_library( SHARE SHARED
	cipher/ecdsa_manager.cpp

	hash/sha_hash.cpp

	#middle_buffer/middle_buffer.cpp
	stream_buffer/stream_buffer.cpp
	stream_buffer/stream_buffer_container.cpp
	stream_buffer/test/unit_test.cpp

	miya_db_client/miya_db_client.cpp
	miya_db_client/miya_db_sb_client.cpp

	# crypto_utils/w_base64/base64.cpp
	# crypto_utils/w_sha/sha.cpp
	# crypto_utils/w_evp_pkey/evp_pkey.cpp
	# crypto_utils/w_rsa/rsa_manager.cpp
	# crypto_utils/w_aes/aes_manager.cpp

	# observer/observer.cpp 仮でss_p2p_node_controllerのobserverを使う
	# observer/observer_strage.cpp 仮でss_p2p_node_controllerのobserverを使う
)

target_link_libraries( SHARE PUBLIC
						${OPENSSL_LIBRARIES}
						# EKP2P #SBSegmentをekp2p::headerから作成するために必要
)

add_definitions( -std=c++20 -w )
