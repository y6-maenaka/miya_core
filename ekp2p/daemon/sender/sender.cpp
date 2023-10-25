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





EKP2PSender::EKP2PSender( int hostSock ,std::shared_ptr<KRoutingTable> kRoutingTable ,std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	_hostSock = hostSock;
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

		std::cout << "sender socket -> " << _hostSock << "\n";

		for(;;)
		{
			popedSB = _incomingSB->popOne();

			switch( popedSB->sendFlag() )
			{

				case static_cast<int>( EKP2P_SEND_UNICAST ):
				{
					std::cout << "destionationAddr 送信" << "\n";
					std::shared_ptr<SocketManager> sockManager = std::make_shared<SocketManager>( popedSB->destinationAddr() , _hostSock );
					std::shared_ptr<EKP2PMessage>  msg = std::make_shared<EKP2PMessage>();

					msg->payload( popedSB->body() , popedSB->bodyLength() ); // コンテンツのセット
					msg->header()->protocol( popedSB->protocol() );
					msg->header()->rpcType( popedSB->rpcType() );
					msg->header()->sourceKNodeAddr( _kRoutingTable->hostNode()->kNodeAddr() ); // 自身のアドレスのセット

					std::cout << "------------------------------" << "\n";
					std::cout << "send  to " << "\n";
					std::cout << "host Sock :: " << _hostSock << "\n";
					std::cout << "ip :: " << inet_ntoa(popedSB->rawSenderAddr().sin_addr) << "\n";
					std::cout << "port :: " << ntohs(popedSB->rawSenderAddr().sin_port) << "\n";
					std::cout << "------------------------------" << "\n";	

					size_t sendLength;
					sendLength = sockManager->send( msg );

					std::cout << "sended message :: " << sendLength << " (bytse)" << "\n";

					break;
				}

				case static_cast<int>(EKP2P_SENDBACK | EKP2P_SEND_UNICAST): // 返信 | ユニキャスト = sourceKNodeに送信
				{
					std::cout << "SourceKNodeAddr に返信" << "\n";
					std::shared_ptr<EKP2PMessage> msg = std::make_shared<EKP2PMessage>();
					std::shared_ptr<SocketManager> sockManager = std::make_shared<SocketManager>( popedSB->rawSenderAddr(), _hostSock ); // rawがいいかSourceがいいか

					msg->payload( popedSB->body() , popedSB->bodyLength() );
					msg->header()->protocol( popedSB->protocol() );
					msg->header()->rpcType( KADEMLIA_RPC_PONG );
					msg->header()->sourceKNodeAddr( _kRoutingTable->hostNode()->kNodeAddr() ); // 自身のアドレスをセット

					std::cout << "------------------------------" << "\n";
					std::cout << "send back to " << "\n";
					std::cout << "ip :: " << inet_ntoa(popedSB->rawSenderAddr().sin_addr) << "\n";
					std::cout << "port :: " << ntohs(popedSB->rawSenderAddr().sin_port) << "\n";
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

					size_t sendLength;
					std::shared_ptr<SocketManager> sockManager;
					std::shared_ptr<EKP2PMessage> msg = std::make_shared<EKP2PMessage>();

					msg->payload( popedSB->body() , popedSB->bodyLength() );
					msg->header()->protocol( popedSB->protocol() );
					msg->header()->rpcType( KADEMLIA_RPC_PONG );
					msg->header()->sourceKNodeAddr( _kRoutingTable->hostNode()->kNodeAddr() ); // 自身のアドレスをセット

					// 1. sourceKNodeに送信
					sockManager = std::make_shared<SocketManager>( popedSB->rawSenderAddr(), _hostSock ); // rawがいいかSourceがいいか

					sendLength = sockManager->send( msg );
					std::cout << "[ send ] :: ( " << sendLength << " ) bytes" << "\n";


					// 2. relayKNodeに送信
					for( auto itr : popedSB->relayKNodeAddrVector() )	{
						sockManager = std::make_shared<SocketManager>(itr, _hostSock );

						sendLength = sockManager->send( msg );
						std::cout << "[ send ] :: ( " << sendLength << " ) bytes" << "\n";
					}
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
