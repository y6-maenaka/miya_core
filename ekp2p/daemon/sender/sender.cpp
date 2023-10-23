#include "sender.h"




#include "../../network/header/header.h"
#include "../../network/message/message.h"

#include "../../network/socket_manager/socket_manager.h"
#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../protocol.h"







namespace ekp2p
{





EKP2PSender::EKP2PSender( std::shared_ptr<KRoutingTable> kRoutingTable ,std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	_kRoutingTable = kRoutingTable;
	_incomingSB = incomingSB;
	_toBrokerSBC = toBrokerSBC;

	std::cout << "EKP2P::daemon::Sender just initialized" << "\n";
	return;
}






int EKP2PSender::start()
{

	if( _incomingSB == nullptr ) return -1;
	if( _kRoutingTable == nullptr ) return -1;
	if( _toBrokerSBC == nullptr )  return -1;

	std::thread ekp2pSender([&]()
	{
		std::cout << "\x1b[32m" << "EKP2P::daemon::Sender ekp2pSender thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); // アクティブなスレッドIDを管理しておく

		std::unique_ptr<SBSegment> popedSB;
		EKP2PSenderOptions options;

		for(;;)
		{		
			std::cout << "<ekp2p::Sender> check 1" << "\n";

			popedSB = _incomingSB->popOne();

			sleep(1);

			std::cout << "<ekp2p::Sender> check 2" << "\n";

			std::cout << "sendFlag =>> " << popedSB->sendFlag() << "\n";

			switch( popedSB->sendFlag() )
			{

				case static_cast<int>(EKP2P_SENDBACK | EKP2P_SEND_UNICAST): // 返信 | ユニキャスト = sourceKNodeに送信
				{
					std::cout << "SourceKNodeAddr に返信" << "\n";
					std::shared_ptr<EKP2PMessage> msg = std::make_shared<EKP2PMessage>();
					std::shared_ptr<SocketManager> sockManager = std::make_shared<SocketManager>(popedSB->sourceKNodeAddr());

					msg->payload( nullptr , 0 );
					msg->header()->rpcType( KADEMLIA_RPC_PONG );
					msg->header()->sourceKNodeAddr( _kRoutingTable->hostNode()->kNodeAddr() ); // 自身のアドレスをセット

					std::cout << "------------------------------" << "\n";
					msg->header()->printRaw();

					std::shared_ptr<unsigned char> exportedMSGHeader; size_t exportedMSGHeaderLength;
					exportedMSGHeaderLength = msg->header()->exportRaw( &exportedMSGHeader );
					std::cout << "exportedMSGHeaderLength >> " << exportedMSGHeaderLength << "\n";

					std::shared_ptr<unsigned char> exportedMSG; size_t exportedMSGLength;
					exportedMSGLength = msg->exportRaw( &exportedMSG );
					std::cout << "exportedMSGLength >> " << exportedMSGLength << "\n";


					struct sockaddr_in inAddr = sockManager->ipv4Addr();

					std::cout << "ip :: " << inet_ntoa( inAddr.sin_addr ) << "\n";
					std::cout << "port :: " << ntohs(popedSB->sourceKNodeAddr()->port()) << "\n";

					std::cout << "------------------------------" << "\n";

					size_t sendLength;
					//sendLength = sockManager->send( popedSB->body() , popedSB->bodyLength() );
					sendLength = sockManager->send( msg );

										
					std::cout << "sended message :: " << sendLength << " (bytes)" << "\n";
					break;
				}


				case static_cast<int>( EKP2P_SENDBACK | EKP2P_SEND_BROADCAST ): // 返信 | ブロードキャスト = sourceKNode + relayKNodeに送信
				{
					std::cout << "SourceKNodeAddr + RelayKNodeAddr に返信" << "\n";
					break;
				}
					
				default:
				{
					std::cout << "一致する送信タイプが存在しません" << "\n";
					break;
				}

			}
		}

	});

	ekp2pSender.detach();
	std::cout << "ekp2pSender thread detached" << "\n";

	return 0;
}






}

