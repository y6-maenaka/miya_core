#ifndef CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A
#define CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A


#include <iostream>
#include <string>
#include <memory>
#include <vector>




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

	void hello(){ std::cout << "HelloWorld" << "\n"; }; 

};


#endif // CE7DDC4F_85EE_4963_9485_A46B4A4C3F5A
