#ifndef F99BC652_963D_45EF_8377_CE50E2B01AC2
#define F99BC652_963D_45EF_8377_CE50E2B01AC2


#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <any>




namespace ekp2p
{
	class KNodeAddr;
};


constexpr unsigned int DEFAULT_STREAM_BUFFER_CAPACITY = 20;



struct SBSegment
{
	struct
	{
		uint8_t _flag;
		uint8_t _type;
		uint16_t _bodySize;
		uint16_t _reserved;
	} _controlBlock;


	struct EKP2PBlock
	{
		std::shared_ptr<ekp2p::KNodeAddr> _sourceNodeAddr; 
		std::vector< std::shared_ptr<ekp2p::KNodeAddr> > _relayKNodeAddrVector;
	} _ekp2pBlock;


	struct
	{
		std::shared_ptr<unsigned char> _body;
	} _sbBlock;

public:
	struct Options
	{
		//std::shared_ptr<std::any> option1;
		std::any option1;
	} options;


	SBSegment(){};
	SBSegment( void* body , unsigned short bodyLength );

	std::shared_ptr<ekp2p::KNodeAddr> sourceKNodeAddr();
	std::shared_ptr<unsigned char> body();
	unsigned short bodyLength()	;
	void body( void* body , unsigned short bodyLength );
	void body( std::shared_ptr<unsigned char> body , unsigned short bodyLength );

	std::shared_ptr<ekp2p::KNodeAddr> sourceNodeAddr();
	std::vector< std::shared_ptr<ekp2p::KNodeAddr> > relayKNodeAddrVector();
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



