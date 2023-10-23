#ifndef B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6
#define B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6


#include <vector>
#include <array>
#include <thread>
#include <memory>

#include <unistd.h>


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;



namespace miya_chain
{


constexpr unsigned short MAX_PROTOCOL = 10;


struct MiyaChainMessage;
struct MiyaChainMessageHeader;



class MiyaChainMessageBrocker
{


private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	// std::shared_ptr<StreamBufferContainer> _destinationSBC;

	std::vector< std::thread::id > _activeSenderThreadIDVector;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {};


public:
	MiyaChainMessageBrocker( std::shared_ptr<StreamBufferContainer> incomingSBC );

	int start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	std::shared_ptr<MiyaChainMessage> parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB );
	std::shared_ptr<StreamBufferContainer> incomingSBC();
};




};

#endif // B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6



