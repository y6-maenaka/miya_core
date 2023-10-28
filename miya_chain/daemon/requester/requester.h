#ifndef A7540500_F3AA_4A7E_9D72_9DD92F911483
#define A7540500_F3AA_4A7E_9D72_9DD92F911483


#include <iostream>
#include <memory>
#include <thread>
#include <vector>


struct SBSegment;
class StreamBufferContainer;





namespace miya_chain
{





class MiyaChainRequester
{
private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<StreamBufferContainer> _toMiyaChainBrokerSBC;

	std::vector<std::thread::id> _activeSenderThreadIDVector; 

public:
	MiyaChainRequester( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toMiyaChainBrokerSBC );
	int start();
};





};



#endif // A7540500_F3AA_4A7E_9D72_9DD92F911483



