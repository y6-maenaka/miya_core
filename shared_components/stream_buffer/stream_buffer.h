#ifndef F99BC652_963D_45EF_8377_CE50E2B01AC2
#define F99BC652_963D_45EF_8377_CE50E2B01AC2


#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>





class KNodeAddr;



constexpr unsigned int DEFAULT_STREAM_BUFFER_CAPACITY = 20;




struct SBSegment
{
	struct
	{
		uint8_t _flag;
		uint8_t _type;
		uint16_t _bodySize;
		uint16_t reserved;
	} _controlBlock;


	struct
	{
		std::shared_ptr<KNodeAddr> _sourceNodeAddr;
		std::vector< std::shared_ptr<KNodeAddr> > _relayKNodeAddrVector;
	} ekp2pBlock;


	struct
	{
		void *_body;
	} _sbBlock;


	SBSegment(){};
	SBSegment( void* body , unsigned short bodyLength );
};







class StreamBuffer
{

private:
	std::mutex _mtx;

	struct 
	{
		std::condition_variable _popCV;
		std::vector< std::condition_variable > _contributers;
	}_popContributer ;

	struct
	{
		std::condition_variable _pushCV;
		std::vector< std::condition_variable > _contributers;
	} _pushContributer;


	struct 
	{
		unsigned int _capacity;
		std::vector< std::unique_ptr<SBSegment> > _queue;
	} _sb;


public:
	StreamBuffer();

	void enqueue( std::unique_ptr<SBSegment> target );
	std::unique_ptr<SBSegment> dequeue();
};



#endif // F99BC652_963D_45EF_8377_CE50E2B01AC2



