#include "sender.h"




#include "../../network/header/header.h"
#include "../../network/socket_manager/socket_manager.h"
#include "../../kademlia/k_node.h"
#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"








namespace ekp2p
{





Sender::Sender( std::shared_ptr<KRoutingTable> kRoutingTable ,std::shared_ptr<StreamBufferContainer> incomingSB )
{
	_kRoutingTable = kRoutingTable;
	_incomingSB = incomingSB;

	std::cout << "EKP2P::daemon::Sender just initialized" << "\n";
	return;
}






void Sender::start()
{
	std::cout << "EKP2P::daemon::Sender ekp2pSender thread started" << "\n";

	std::thread ekp2pSender([&]()
	{
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); // アクティブなスレッドIDを管理しておく

		std::unique_ptr<SBSegment> popedSB;
		EKP2PSenderOptions options;

		for(;;)
		{		
			std::cout << "<ekp2p::Sender> check 1" << "\n";

			popedSB = _incomingSB->popOne();

			std::cout << "<ekp2p::Sender> check 2" << "\n";

			options = std::any_cast<EKP2PSenderOptions>( popedSB->options.option1 );

			std::cout << "<ekp2p::Sender> check 3" << "\n";

			switch( options.castType )
			{
				case EKP2P_SEND_UNICAST: // ユニキャスト 送信時にはルーティングテーブルの更新はしない
				{
					std::shared_ptr<SocketManager> sockManager = std::make_shared<SocketManager>(options.destinationNodeAddr.at(0));
					sockManager->send( popedSB->body() , popedSB->bodyLength() );
					return;
				}


				case EKP2P_SEND_BROADCAST:
				{
					int castSize = options.castSize;
					std::vector<KClientNode> clients; // bucketの先頭から送信する
					
					for( auto itr : clients )
					{
						itr.socketManager()->send( popedSB->body(), popedSB->bodyLength() );
					}
					return;
				}
			}
		}

	});

	ekp2pSender.detach();
	std::cout << "ekp2pSender thread detached" << "\n";

}






}

