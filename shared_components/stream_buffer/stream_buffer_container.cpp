#include "stream_buffer_container.h"

#include "./stream_buffer.h"


StreamBufferContainer::StreamBufferContainer()
{

	StreamBuffer *newStreamBuffer = new StreamBuffer;
	_sbs.push_back( newStreamBuffer );

}
