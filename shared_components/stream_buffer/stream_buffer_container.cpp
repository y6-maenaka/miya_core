#include "stream_buffer_container.h"

#include "./stream_buffer.h"





void StreamBufferContainer::pushOne( std::unique_ptr<SBSegment> target )
{
	_sbs.at(0)->enqueue( std::move(target) );
}







std::unique_ptr<SBSegment> StreamBufferContainer::popOne()
{
	return _sbs.at(0)->dequeue();
}





StreamBufferContainer::StreamBufferContainer( std::function<void()> scaleOutHandler , std::function<void()> scaleDownHandler )
{
	StreamBuffer *newStreamBuffer = new StreamBuffer;
	_sbs.push_back( newStreamBuffer );
	_bufferCount = 1;


	_scaleOutRequest = scaleOutHandler;
	_scaleDownRequest = scaleDownHandler;
}
