#include "node_0.h"


#include "../../control_interface/control_interface.h" // コントロールインターフェース


int main()
{
	std::cout << "Test Node 1 Launghed" << "\n";
	std::mutex mtx;
	std::condition_variable cv;

	std::shared_ptr<ekp2p::EKP2P> ekp2p = std::make_shared<ekp2p::EKP2P>();
	ekp2p->init( "../.config/stun_servers.json" );


	sleep(1);

	std::shared_ptr<miya_chain::MiyaChainManager> chainManager = std::make_shared<miya_chain::MiyaChainManager>();
	chainManager->init( ekp2p->toBrokerSBC() );
	ekp2p->assignBrokerDestination( chainManager->toBrokerSBC() , 2 ); // ekp2pのプロトコル転送先にmiyaChainモジュールを設定


	ekp2p->start( true ); // ekp2pの起動
	chainManager->start(); // miyaChainManagerの起動


	std::unique_lock<std::mutex> lock(mtx);
	cv.wait( lock );


	std::cout << "==============================================================" << "\n";

	std::cout << "main thread start waiting ... " << "\n";
}
