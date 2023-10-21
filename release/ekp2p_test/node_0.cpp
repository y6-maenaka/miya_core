#include "node_0.h"


#include "../../control_interface/control_interface.h" // コントロールインターフェース



int main()
{
	std::cout << "This is Node0" << "\n";

	std::shared_ptr<ekp2p::EKP2P> ekp2p = std::make_shared<ekp2p::EKP2P>();
	ekp2p->init();
	ekp2p->start( true );

	std::cout << "start done" << "\n";

	sleep(8);

	std::cout << "==============================================================" << "\n";
}
