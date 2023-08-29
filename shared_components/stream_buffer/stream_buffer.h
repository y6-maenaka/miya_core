#ifndef F99BC652_963D_45EF_8377_CE50E2B01AC2
#define F99BC652_963D_45EF_8377_CE50E2B01AC2


#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>





class KNodeAddr;






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
		void *_bpdy;
	} _sbBlock;

};






class StreamBuffer
{

private:
	std::mutex _mtx;
	std::condition_variable _cv;

	unsigned int _capacity;
	std::vector< std::unique_ptr<SBSegment> > _sb;



public:
	void pushOne( std::unique_ptr<SBSegment> target );
	std::unique_ptr<SBSegment> popOne();

};



#endif // F99BC652_963D_45EF_8377_CE50E2B01AC2



