#include "stream_buffer_container.h"

#include "./stream_buffer.h"





void StreamBufferContainer::pushOne( std::unique_ptr<SBSegment> target )
{
	// 複数のstreamBufferが追加される
	_sbs.at(0)->enqueue( std::move(target) );
}







std::unique_ptr<SBSegment> StreamBufferContainer::popOne()
{
	return _sbs.at(0)->dequeue();
}





StreamBufferContainer::StreamBufferContainer( std::function<void()> scaleOutHandler , std::function<void()> scaleDownHandler )
{
	std::shared_ptr<StreamBuffer> newStreamBuffer = std::make_shared<StreamBuffer>();
	_sbs.push_back( newStreamBuffer );
	_bufferCount = 1;

	if( scaleOutHandler != nullptr )
		_scaleOutRequest = scaleOutHandler;

	if( scaleDownHandler != nullptr )
		_scaleDownRequest = scaleDownHandler;
}
