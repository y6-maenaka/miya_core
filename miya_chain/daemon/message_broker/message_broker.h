#ifndef B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6
#define B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6


#include <vector>
#include <array>


class StreamBuffer;
class StreamBufferContainer;



namespace miya_chain
{


constexpr unsigned short MAX_PROTOCOL = 10;


class MiyaChainMessageBrocker
{


private:
	std::shared_ptr<StreamBufferContainer> _sourceSBC;
	std::shared_ptr<StreamBufferContainer> _destinationSBC;

	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {};


public:
	void start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

};




};

#endif // B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6



