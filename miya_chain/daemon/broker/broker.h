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





class MiyaChainMSGRouter
{
public:
	unsigned short fowardingSBCTo(); // メッセージを解析して転送するSBCIDを得る
};








class MiyaChainMessageBrocker
{


private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<StreamBufferContainer> _toEKP2PBrokerSBC;
	// std::shared_ptr<StreamBufferContainer> _destinationSBC;

	std::vector< std::thread::id > _activeSenderThreadIDVector;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {};


public:
	MiyaChainMessageBrocker( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC );

	int start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	std::shared_ptr<MiyaChainMessage> parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB );
	std::shared_ptr<StreamBufferContainer> incomingSBC();

	std::shared_ptr<MiyaChainMessage> parseRawMiyaChainMSG( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
};




};

#endif // B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6



