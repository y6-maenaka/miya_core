#ifndef F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55
#define F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55


#include <memory>
#include <iostream>



struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;






namespace miya_chain
{


class MiyaChainMessageBrocker;



class MiyaChainManager
{

private:

	struct 
	{
		std::shared_ptr<StreamBufferContainer> _toBrokerSBC;
		std::shared_ptr<MiyaChainMessageBrocker> _broker;
	} _brokerDaemon;


public:
	int init();
	int start();

	std::shared_ptr<StreamBufferContainer> toBrokerSBC();


};




};




#endif // F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55



