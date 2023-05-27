#include "middle_buffer.h"



unsigned int BufferSegment::exportRaw( unsigned char **ret )
{

	unsigned int retSize = sizeof(_label) + _content._bodySize + sizeof(_delimiter);

	*ret = new unsigned char[retSize];
	memcpy( ret , &_label , sizeof(_label) );
	memcpy( ret + sizeof(_label), _content._body , sizeof(_content._bodySize) );
	memcpy( ret + sizeof(_label) + _content._bodySize , &_delimiter , sizeof(_delimiter) );

	return retSize;
}



void BufferSegment::importRaw( unsigned char *rawBufferSegment )
{ 
	memcpy( &_label , rawBufferSegment , sizeof(_label) ); _label = ntohs( _label );

	_content._body = new unsigned char[_label];
	memcpy( &_content._body , rawBufferSegment + sizeof(_label) , static_cast<size_t>(_label) );
	_content._bodySize = static_cast<size_t>(_label);

	memcpy( &_delimiter , rawBufferSegment + sizeof(_label) + static_cast<size_t>(_label), sizeof(_delimiter) );

}

bool BufferSegment::validate()
{
	// もうすこしちゃんとした処理を書く
		
	if( _delimiter != DELIMITER ) return false;
	return true	;
}




// return 空き容量
int MiddleBuffer::pushOne( unsigned char *segment , unsigned int segmentSize , bool isBlocking )
{

	std::unique_lock<std::mutex> lock(_mtx); // ロックの獲得

	if( (_offset) + segmentSize  >= _bufferSize  )	// 追加するとバッファオーバーフローする場合
	{ 
		if( isBlocking ) _cv.wait( lock );
		else return -1;
	}

	/* バッファに挿入 */
	BufferSegment bufferSegment( segment , segmentSize );
	unsigned char* rawSegment; unsigned int rawSegmentSize;
	rawSegmentSize = bufferSegment.exportRaw( &rawSegment );

	memcpy( _buffer + _offset , rawSegment , rawSegmentSize );
	_offset += rawSegmentSize;

	delete rawSegment;

	return _bufferSize - _offset; //　鍵の解放

}



// return 空き容量
int MiddleBuffer::popOne( unsigned char **segment , unsigned int *segmentSize , bool isBlocking  )
{ 
	*segmentSize = 0;

	std::unique_lock<std::mutex> lock(_mtx);

	if( _offset == 0  )
	{ // バッファが空だった
		if( isBlocking ) _cv.wait( lock );
		else  return 0;
	}

	if( (*segment = new unsigned char[_offset]) == nullptr ){ // 領域確保エラー
		return -1;
	}

	/* segmentSize の取得 */
	BufferSegment bufferSegment;
	memcpy( &bufferSegment._label , _buffer , sizeof(bufferSegment._label) ); bufferSegment._label = ntohs( bufferSegment._label );

	if( !(bufferSegment._label <= std::numeric_limits<uint16_t>::max() && bufferSegment._label >= std::numeric_limits<uint16_t>::min()) ){
		_offset = 0; // バッファ内がぐちゃぐちゃになっている可能性があるのでここでは簡単にバッファを論理的にリフレッシュする
		return _bufferSize - _offset;
	}

	unsigned char* rawBufferSegment = new unsigned char[ bufferSegment._label ];
	memcpy( rawBufferSegment , _buffer , static_cast<size_t>(bufferSegment._label) );
	bufferSegment.importRaw( rawBufferSegment );
	delete[] rawBufferSegment;

	if( !bufferSegment.validate() ){
		_offset = 0; // バッファ内がぐちゃぐちゃになっている可能性があるのでリフレッシュする
		return _bufferSize - _offset;
	}
	
	memmove( _buffer , _buffer + bufferSegment._label , bufferSegment._label ); // 1segment分詰める
	_offset -= bufferSegment._label;

	*segment = bufferSegment._content._body;
	// bufferSegment自体は勝手に解放される
	return _bufferSize - _offset;

}


