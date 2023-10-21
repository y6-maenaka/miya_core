#ifndef F866A63B_BCF1_4114_BBB7_D9D6FCDF93B5
#define F866A63B_BCF1_4114_BBB7_D9D6FCDF93B5

#include <iostream>
#include <memory>
#include <thread>
#include <vector>




class StreamBuffer;
class StreamBufferContainer;




namespace ekp2p
{


class KBucket;
class KRoutingTable;
class KClientNode;





class EKP2PKRoutingTableUpdator
{


private:
	// KRoutingTable	*_routingTable;
	std::shared_ptr<KRoutingTable> _routingTable;
	std::shared_ptr<StreamBufferContainer> _incomingSB;
	std::shared_ptr<StreamBufferContainer> _toBrokerSBC;

	struct KClientNodeSwapWaitQueue
	{
	} _kClientNodeSwapWaitQueue;

	std::vector<std::thread::id> _activeSenderThreadIDVector; // 念の為管理しておく

public:
	EKP2PKRoutingTableUpdator( std::shared_ptr<KRoutingTable> routingTable , std::shared_ptr<StreamBufferContainer> incomingSB , std::shared_ptr<StreamBufferContainer> toBrokerSBC );

	// void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target );
	// void setSourceStreamBuffer( std::shared_ptr<StreamBufferContainer> target );

	int start();
};





};






#endif // F866A63B_BCF1_4114_BBB7_D9D6FCDF93B5



