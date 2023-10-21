#include "k_client_node_swap_wait_queue.h"




#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_routing_table.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"




namespace ekp2p
{






SwapWaitNodePair::SwapWaitNodePair( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode )
{
	_startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	_remainingNode = remainingNode;
	_candidateNode = candidateNode;
	_targetBucket = targetBucket;
	return;
}










KClientNodeSwapWaitQueue::KClientNodeSwapWaitQueue()
{
	_sbc = std::make_shared<StreamBufferContainer>();
}




void KClientNodeSwapWaitQueue::start()
{
	
	std::thread pingWait([&]()
	{
		std::unique_ptr<SBSegment> sb;
		SwapWaitNodePair targetPair;


		for(;;)	
		{
			std::unique_lock<std::mutex> lock(_mtx);
			//sb = _sbc->popOne();
			//targetPair = std::any_cast<SwapWaitNodePair>(sb->options.option1);
	
			_cv.wait_for( lock , std::chrono::seconds(DEFAULT_WAIT_PING_TIME) , [&]{ // 第3引数が満たされたら解除
				return (_nodePairVector.size() == 0) || 
								(( _nodePairVector.at(0)._startTime - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() ) >= DEFAULT_WAIT_PING_TIME);
			});

			for( std::vector<SwapWaitNodePair>::iterator itr = _nodePairVector.begin(); itr != _nodePairVector.end(); itr++ )
			{
				if((itr->_startTime	-  std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() ) >= DEFAULT_WAIT_PING_TIME ) _nodePairVector.erase(itr);
				else break;
			}
			_cv.wait( lock );
		}

	});
	
}




void KClientNodeSwapWaitQueue::regist( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode )
{
	std::unique_lock<std::mutex> lock(_mtx);

	SwapWaitNodePair newPair( targetBucket , remainingNode , candidateNode );
	_nodePairVector.push_back( newPair );
}




void KClientNodeSwapWaitQueue::unregist( std::shared_ptr<KClientNode> target )
{
	std::unique_lock<std::mutex> lock(_mtx);


	auto customCompare = [target]( SwapWaitNodePair n2 ){
		return (memcmp( target->kNodeAddr()->ID() ,n2._remainingNode->kNodeAddr()->ID() , 20 ) == 0);
	};

	std::vector<SwapWaitNodePair>::iterator targetItr = std::find_if( _nodePairVector.begin(), _nodePairVector.end(), customCompare );
	_nodePairVector.erase( targetItr );
	_cv.notify_all();
}








};
