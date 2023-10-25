#ifndef F0AFAB39_4277_4203_8070_9932D0C44BA2
#define F0AFAB39_4277_4203_8070_9932D0C44BA2


#include <iostream>
#include <memory>
#include <thread>


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;


namespace ekp2p{


struct StunResponse;




class StunRequestHandlerDaemon
{
private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<StreamBufferContainer> _toBrokerSBC;

public:
	StunRequestHandlerDaemon( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toBrokerSBC );
	int start();
};




}; // close ekp2p namespace



#endif // F0AFAB39_4277_4203_8070_9932D0C44BA2
