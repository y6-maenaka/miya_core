#include "__test.h"

#include "./k_client_node_swap_wait_queue.h"



namespace ekp2p
{




int ping_wait_queue_unit_test()
{
	std::cout << "Hello World" << "\n";


	ekp2p::KClientNodeSwapWaitQueue waitQueue;
	waitQueue.start();
	
	sleep(1);
	//waitQueue.regist( nullptr , nullptr , nullptr );
	//waitQueue.regist( nullptr , nullptr , nullptr );


	sleep(20);
	// waitQueue.regist( nullptr , nullptr , nullptr );
	

	return 0;
}


}
