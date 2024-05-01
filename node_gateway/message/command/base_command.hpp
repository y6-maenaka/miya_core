#ifndef DC8B8BFF_FE59_4105_A0C9_5984D4E0FC54
#define DC8B8BFF_FE59_4105_A0C9_5984D4E0FC54


#include <memory>


namespace chain
{


struct MiyaChainPayloadFunction
{
public:
	virtual size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ) = 0;
	virtual bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ) = 0;
};


}


#endif // 


