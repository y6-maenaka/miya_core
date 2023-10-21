#ifndef F866A63B_BCF1_4114_BBB7_D9D6FCDF93B5
#define F866A63B_BCF1_4114_BBB7_D9D6FCDF93B5

#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>





class StreamBuffer;
class StreamBufferContainer;




namespace ekp2p
{


class KBucket;
class KRoutingTable;
class KClientNode;





struct SwapWaitNodePair
{
	std::shared_ptr<KBucket> _targetBucket;

	std::shared_ptr<KClientNode> _remainingNode;
	std::shared_ptr<KClientNode> _candidateNode;

	std::chrono::seconds waitfor;
};


struct KClientNodeSwapWaitQueue
{
private:
	std::vector<SwapWaitNodePair> _nodePairVector;
	std::mutex _mtx;
	std::condition_variable _cv;


public:
	void start(); // start with thread

	void regist( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode );
};







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



