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








class MiyaChainBrocker
{


private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<StreamBufferContainer> _toEKP2PBrokerSBC;
	// std::shared_ptr<StreamBufferContainer> _destinationSBC;

	std::vector< std::thread::id > _activeSenderThreadIDVector;
	std::array< std::shared_ptr<StreamBufferContainer> , MAX_PROTOCOL > _sbHub = {};


public:
	// これはアクセス可能とする
	std::array< bool , MAX_PROTOCOL > allowedForwardingFilter; // 転送するSBCのフィルター trueで許可
	void allowAll();
	void blockAll();


	MiyaChainBrocker( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC );

	int start();
	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination );

	static std::shared_ptr<MiyaChainMessage> parseRawMiyaChainMessage( std::shared_ptr<SBSegment> fromSB );

	/* Getter */
	std::shared_ptr<StreamBufferContainer> incomingSBC();

	int forwardingDestination( std::shared_ptr<StreamBufferContainer> sbc , unsigned short destination );
};




};

#endif // B5C1EEE3_730A_44F5_BCDF_4B73ACF8CFA6



