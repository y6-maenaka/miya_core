#ifndef D08619B0_7F06_4EE7_9956_A163FB949AE8
#define D08619B0_7F06_4EE7_9956_A163FB949AE8


#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <functional>
#include <cstdint>


constexpr uint16_t DELIMITER = 0x0020;



struct BufferSegment
{
	uint16_t _label;

	struct content{
		unsigned char* _body;
		unsigned int _bodySize;

	} _content ;

	uint16_t _delimiter;


	BufferSegment( void* body , unsigned int bodySize ) : _delimiter(0x0020){
		_content._body = static_cast<unsigned char*>(body); _content._bodySize = bodySize;
		_label = htons( bodySize );
	}
	BufferSegment(){};


	unsigned int exportRaw( unsigned char **ret );
	void importRaw( unsigned char *rawBufferSegment );

	bool validate();
};




class MiddleBuffer
{

private:
	std::mutex _mtx;
	std::condition_variable _cv;

	unsigned char *_buffer;
	unsigned int _offset;

	unsigned int _bufferSize;


public:

	//void bufferControl( std::function<int( void*, unsigned int, MiddleBuffer*)> func , void *segment, unsigned int segmentSize, bool isBlocking = true );

	int pushOne( unsigned char *segment , unsigned int segmentSize , bool isBlocking = true );
	int popOne( unsigned char **segment , unsigned int *segmentSize , bool isBlocking = true );


	MiddleBuffer( unsigned int bufferSize )
	{
		_bufferSize = bufferSize;

		_buffer = new unsigned char[bufferSize];
		memset( _buffer , 0x0 , bufferSize );

		_offset = 0;
	};

	~MiddleBuffer(){
		delete _buffer;
	}


};


#endif // D08619B0_7F06_4EE7_9956_A163FB949AE8

