#ifndef AA6B6A73_8ED4_4735_9E81_2934DF646488
#define AA6B6A73_8ED4_4735_9E81_2934DF646488



#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <functional>



namespace miya_db{



class MiddleBuffer
{

private:
	std::mutex _mtx;
	std::condition_variable _cv;

	unsigned char *_buffer;
	int _offset;

	unsigned int _bufferSize;


public:

	void bufferControl( std::function<int( void*, unsigned int, MiddleBuffer*)> func , void *segment, unsigned int segmentSize, bool isBlocking = true );

	friend int push( void *segment , unsigned int segmentSize , MiddleBuffer* _midBuffer );
	friend int pop( void *segment , unsigned int segmentSize , MiddleBuffer* _midBuffer );


	MiddleBuffer( unsigned int bufferSize )
	{
		_bufferSize = bufferSize;

		_buffer = new unsigned char[bufferSize];
		memset( _buffer , 0x0 , bufferSize );

		_offset = 0;
	};

};




}; // close miya_db namespace

#endif // AA6B6A73_8ED4_4735_9E81_2934DF646488

