#ifndef B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6
#define B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6


#include <vector>
#include <array>
#include <thread>
#include <memory>


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
	std::shared_ptr<StreamBufferContainer> _sourceSBC;
	// std::shared_ptr<StreamBufferContainer> _destinationSBC;

	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {};


public:
	void start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	std::shared_ptr<MiyaChainMessage> parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB );

};




};

#endif // B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6



