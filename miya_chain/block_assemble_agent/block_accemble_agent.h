#ifndef F3BB8106_1DDE_4AE6_8217_0A51255DB415
#define F3BB8106_1DDE_4AE6_8217_0A51255DB415



#include <memory>
#include <vector>



namespace ekp2p
{
	class KNodeAddr;
};




namespace miya_chain
{




class BlockAssembleAgent
{
private:

	std::shared_ptr<unsigned char> _requestBlockHash;

	struct
	{
		std::shared_ptr<ekp2p::KNodeAddr>	_sourceNodeAddr;
		std::vector< std::shared_ptr<ekp2p::KNodeAddr> > _relayKNodeAddrVector;
	} _networks;


public:
	BlockAssembleAgent( std::shared_ptr<unsigned char> targetBlockHash );
	void autoRequest();

};



};

#endif //

