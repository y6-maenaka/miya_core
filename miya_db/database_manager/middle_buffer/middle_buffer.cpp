#include "middle_buffer.h"



namespace miya_db{




void MiddleBuffer::bufferControl( std::function<int( void*, unsigned int , MiddleBuffer*) func, void *segment , unsigned int segmentSize, bool isBlocking )
{
	int flag;
	std::unique_lock<std::mutex> lock(_mtx);  // ロックの獲得 or wait

	flag = callback( segment , segmentSize , this ); // 一応明示的にthisポインタを渡す

	if( flag < 0 && isBlocking ){
		_cv.wait( lock );
		callback( segment , segmentSize , this );

	}else{
		_cv.notify_one(); 
	}	
}





int push( void *segment , unsigned int segmentSize , MiddleBuffer* _midBuffer )
{
	if( (_midBuffer->offset) + segmentSize  >= _midBuffer->_bufferSize  )	{
		return -1;
	}

	memcpy( _midBuffer->_buffer + _midBuffer->_offSet  ,  segment , segmentSize );
	_midBuffer->_offSet += segmentSize;

	return 0;

}



int pop( void *segment , unsigned int segmentSize , MiddleBuffer *_midBuffer )
{

}


}; // close miya_db namespace
