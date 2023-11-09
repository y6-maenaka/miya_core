#ifndef CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A
#define CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A


#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "../../shared_components/json.hpp"


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;





class MiyaDBSBClient
{


private:
	std::shared_ptr<StreamBufferContainer> _pushSBContainer = nullptr;
	std::shared_ptr<StreamBufferContainer> _popSBContainer = nullptr;


public:
	MiyaDBSBClient( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer );

	size_t get( std::shared_ptr<unsigned char> rawKey ,std::shared_ptr<unsigned char> *retRaw );
	bool add( std::shared_ptr<unsigned char> rawKey , std::shared_ptr<unsigned char> rawValue , size_t rawValueLength );
	bool remove( std::shared_ptr<unsigned char> rawKey );

	bool safeMode();
	bool commit();
	bool abort();

	size_t dumpRaw( nlohmann::json target , std::shared_ptr<unsigned char> *retRaw );
	std::unique_ptr<SBSegment> generateQuerySB( nlohmann::json queryJson );

	// 不完全な実装 要修正
	nlohmann::json filterResponseSB( nlohmann::json queryJson );

	void hello(){ std::cout << "HelloWorld" << "\n"; }; 

};


#endif // CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A
