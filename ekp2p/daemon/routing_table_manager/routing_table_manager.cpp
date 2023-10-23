#include "routing_table_manager.h"

#include "../sender/sender.h"

#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../ekp2p.h"
#include "../../protocol.h"


namespace ekp2p
{



EKP2PKRoutingTableUpdator::EKP2PKRoutingTableUpdator( std::shared_ptr<KRoutingTable> routingTable , std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	_incomingSB = incomingSB;
	_routingTable = routingTable;
	_toBrokerSBC = toBrokerSBC;

	_swapWaitQueue.start();


	// 通知関数のバインドと設定
	std::function<void( std::shared_ptr<KBucket> , std::shared_ptr<KClientNode>, std::shared_ptr<KClientNode>) > notifyMethod = std::bind(
					&KClientNodeSwapWaitQueue::regist , std::ref(_swapWaitQueue) , std::placeholders::_1 , std::placeholders::_2, std::placeholders::_3 
			);
	_routingTable->notifyNodeSwap( notifyMethod );

	std::cout << "EKP2P::daemon::KRoutingTableUpdator just initialized" << "\n";
}






int EKP2PKRoutingTableUpdator::start()
{
	if( _routingTable == nullptr ) return -1;
	if( _incomingSB == nullptr ) return -1;
	if( _toBrokerSBC == nullptr ) return -1;


	std::thread ekp2pKRoutingTableUpdator([&]()
	{
		std::cout <<  "\x1b[32m" << "EKP2P::daemon::KRoutingTableUpdator ekp2pKRoutingTableUpdator thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 
		
		int discardFlag = false;
		std::unique_ptr<SBSegment> popedSB ;
		for(;;)
		{
			popedSB = _incomingSB->popOne();
			std::cout << "rpc Type :: " << popedSB->rpcType() << "\n";
			
			// handle ekp2p msg		
			switch( popedSB->rpcType() )
			{
				case static_cast<int>(KADEMLIA_RPC_IGNORE): // IGNORE
				{
					std::cout << "[ IGNORE ] KADEMLIA " << "\n";
					break;
				} 

				case static_cast<int>(KADEMLIA_RPC_PING): // PING
				{
					std::cout << "[ PING ] KADEMLIA " << "\n";
					popedSB->sendFlag( EKP2P_SENDBACK | EKP2P_SEND_UNICAST ); // 送信フラグを単一センドバックに
					std::cout << "destinoation :: " << popedSB->forwardingSBCID() << "\n";

					if( popedSB->body() == nullptr || popedSB->bodyLength() <= 0 ) // 仮想へのデータが特になければsenderへ転送する
						popedSB->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER );
					std::cout << "discardFlag :: " << discardFlag << "\n";
					// 単純にPONGをレスポンスする
					break;
				}	

				case static_cast<int>(KADEMLIA_RPC_PONG): // PONG
				{
					std::cout << "[ PONG ] KADEMLIA " << "\n";
					std::shared_ptr<KClientNode> pongedNode = std::make_shared<KClientNode>( popedSB->sourceKNodeAddr() );
					_swapWaitQueue.unregist( pongedNode );

					if( popedSB->bodyLength() <= 0 ) discardFlag = false; // 特に下層へのデータがなければセグメントは転送せずに破棄する
					break;
				}

				case static_cast<int>(KADEMLIA_RPC_FIND_NODE): // FIND_NODE
				{
					std::cout << "[ FIND_NODE ] KADEMLIA " << "\n";
					break;
				}

			}
			
			popedSB->ekp2pIsProcessed( true );  // 処理済みか否か 必ず書く

			if( !discardFlag ){ // if not need discard
				std::cout << "departure from KRoutingTableManager to Broker" << "\n";
				_toBrokerSBC->pushOne( std::move(popedSB) );
			}
		}
	});

	ekp2pKRoutingTableUpdator.detach();
	std::cout << "ekp2pKRoutingTableUpdator thread detached" << "\n";


	return 0;

}











};
