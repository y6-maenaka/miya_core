#include "stream_buffer_container.h"

#include "./stream_buffer.h"





void StreamBufferContainer::pushOne( std::unique_ptr<SBSegment> target , size_t timeout )
{
	// 複数のstreamBufferが追加される
	_sbs.at(0)->enqueue( std::move(target) , timeout );
}







std::unique_ptr<SBSegment> StreamBufferContainer::popOne( size_t timeout )
{
	return _sbs.at(0)->dequeue( timeout );
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
