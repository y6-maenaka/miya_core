#include "k_client_node_swap_wait_queue.h"




#include "../../kademlia/k_node.h"
#include "../../kademlia/k_routing_table/k_bucket.h"
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
	_nodePairVector.clear();
}




void KClientNodeSwapWaitQueue::start()
{

	std::thread pingWait([ this ]()
	{
		std::unique_ptr<SBSegment> sb;
		SwapWaitNodePair targetPair;


		for(;;)	
		{
			std::unique_lock<std::mutex> lock(_mtx);

			remainSwapPair:	
			_cv.wait_for( lock , std::chrono::seconds(DEFAULT_WAIT_PING_TIME) , [&]{ // 第3引数が満たされたら(true)解除
					bool flag = (_nodePairVector.size() == 0) ? true : ((std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()) - _nodePairVector.at(0)._startTime) >= DEFAULT_WAIT_PING_TIME;
					return flag;
			});


			for( std::vector<SwapWaitNodePair>::iterator itr = _nodePairVector.begin(); itr != _nodePairVector.end();)
			{
				if(( std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - itr->_startTime	) >= DEFAULT_WAIT_PING_TIME ){
					itr->_targetBucket->swapForce( itr->_remainingNode , itr->_candidateNode ); // 指定時間が経過したので交換する
					itr = _nodePairVector.erase(itr);
					std::cout << "erased !!!!" << "\n";
				}
				else break;
			}

	
			if( _nodePairVector.size() > 0 ) 
				goto remainSwapPair;

			_cv.wait( lock );
		}

	});
	
	pingWait.detach(); // 起動元スレッドが終了するとキャプチャできなくなるので注意
	//pingWait.join();
	std::cout << "pingWait thread detached" << "\n";
	
}




void KClientNodeSwapWaitQueue::regist( std::shared_ptr<KBucket> targetBucket, std::shared_ptr<KClientNode> remainingNode , std::shared_ptr<KClientNode> candidateNode )
{
	std::unique_lock<std::mutex> lock(_mtx);

	SwapWaitNodePair newPair( targetBucket , remainingNode , candidateNode );
	_nodePairVector.push_back( newPair );

	_cv.notify_all();
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
