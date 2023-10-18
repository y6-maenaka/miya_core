#include "node_0.h"





int main()
{

	std::shared_ptr<ekp2p::EKP2P> ekp2p = std::make_shared<ekp2p::EKP2P>();
	ekp2p->init();
	ekp2p->start();


	std::cout << "==============================================================" << "\n";
}
