#ifndef A9E10781_5566_44D6_8FC8_AC360E8A2474
#define A9E10781_5566_44D6_8FC8_AC360E8A2474




#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <any>





class StreamBuffer;
class StreamBufferContainer;





namespace ekp2p
{



constexpr unsigned int DEFAULT_WAIT_PING_TIME = 5;





class KBucket;
class KRoutingTable;
class KClientNode;






struct SwapWaitNodePair
{
// all publicで
	std::shared_ptr<KBucket> _targetBucket;

	std::shared_ptr<KClientNode> _remainingNode;
	std::shared_ptr<KClientNode> _candidateNode;

	long long _startTime;

	SwapWaitNodePair(){};
	SwapWaitNodePair( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode );
};





class KClientNodeSwapWaitQueue
{
private:
	std::vector<SwapWaitNodePair> _nodePairVector;
	std::shared_ptr<StreamBufferContainer> _sbc;

	std::mutex _mtx;
	std::condition_variable _cv;


public:
	KClientNodeSwapWaitQueue();
	void start(); // start with thread

	void regist( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode );
	void unregist( std::shared_ptr<KClientNode> target );
};







};



#endif // A9E10781_5566_44D6_8FC8_AC360E8A2474



