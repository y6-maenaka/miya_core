#ifndef E48A5934_C850_47B6_9B86_58FDE95D1169
#define E48A5934_C850_47B6_9B86_58FDE95D1169



#include <memory>


class StreamBufferContainer;


namespace ekp2p
{


class KRoutingTable;





class KRoutingTableUpdator
{


private:
	KRoutingTable	*_routingTable;

	std::shared_ptr<StreamBufferContainer> _sourceSBC;
	std::shared_ptr<StreamBufferContainer> _destinationSBC;

public:
	KRoutingTableUpdator( KRoutingTable *target = nullptr );

	void setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target );
	void setSourceStreamBuffer( std::shared_ptr<StreamBufferContainer> target );

	void start();
};







};



#endif // E48A5934_C850_47B6_9B86_58FDE95D1169



