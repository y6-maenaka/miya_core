#include "stream_buffer.h"

#include "../../ekp2p/kademlia/k_node.h"
#include "../../ekp2p/network/header/header.h"



SBSegment::SBSegment( void* body , unsigned short bodyLength )
{
	if( body == nullptr || bodyLength <= 0 ) return;

	memset( this , 0x00 , sizeof(struct SBSegment) );

	_controlBlock._bodySize = static_cast<uint16_t>(bodyLength);
	_sbBlock._body = std::make_shared<unsigned char>( *((unsigned char*)(body)) );
}





std::shared_ptr<ekp2p::KNodeAddr> SBSegment::sourceKNodeAddr()
{
	return _ekp2pBlock._sourceKNodeAddr;
}



std::shared_ptr<unsigned char> SBSegment::body()
{
	return _sbBlock._body;
}




unsigned short SBSegment::bodyLength()
{
	return static_cast<unsigned short>(_controlBlock._bodySize);
}


void SBSegment::body( void* body , unsigned short bodyLength )
{
	_controlBlock._bodySize = static_cast<uint16_t>(bodyLength);
	_sbBlock._body = std::make_shared<unsigned char>( *((unsigned char*)(body)) );

}
void SBSegment::body( std::shared_ptr<unsigned char> body , unsigned short bodyLength )
{
	_controlBlock._bodySize = static_cast<uint16_t>(bodyLength);
	_sbBlock._body = body;
}




std::vector< std::shared_ptr<ekp2p::KNodeAddr> > SBSegment::relayKNodeAddrVector()
{
	return _ekp2pBlock._relayKNodeAddrVector;
}


unsigned short SBSegment::rpcType()
{
	return _ekp2pBlock._rpcType;
}

unsigned short SBSegment::protocol()
{
	return _controlBlock._protocol;
}

bool SBSegment::ekp2pIsProcessed()
{
	return _ekp2pBlock._isProcessed;
}

int SBSegment::sendFlag()
{
	return _ekp2pBlock._sendFlag;
}

unsigned short SBSegment::forwardingSBCID()
{
	return _controlBlock._forwardingSBCID;
}

unsigned char SBSegment::flag()
{
	return _controlBlock._flag;
}

struct sockaddr_in SBSegment::rawClientAddr()
{
	return _ekp2pBlock._rawClientAddr;
}




void SBSegment::sourceKNodeAddr( std::shared_ptr<ekp2p::KNodeAddr> target )
{
	_ekp2pBlock._sourceKNodeAddr = target;
}


void SBSegment::relayKNodeAddrVector( std::vector< std::shared_ptr<ekp2p::KNodeAddr> > target )
{
	_ekp2pBlock._relayKNodeAddrVector = target;
}


void SBSegment::relayKNodeAddrVector( std::shared_ptr<ekp2p::KNodeAddr> target )
{
	_ekp2pBlock._relayKNodeAddrVector.push_back( target );
}


void SBSegment::rpcType( unsigned short target )
{
	_ekp2pBlock._rpcType = target;
}

void SBSegment::protocol( unsigned short target )
{
	_controlBlock._protocol = target;
}

void SBSegment::ekp2pIsProcessed( bool target )
{
	_ekp2pBlock._isProcessed = target;
}


void SBSegment::sendFlag( int target )
{
	_ekp2pBlock._sendFlag = target;
}

void SBSegment::forwardingSBCID( unsigned short target )
{
	_controlBlock._forwardingSBCID = target;
}

void SBSegment::flag( unsigned char target )
{
	_controlBlock._flag = target;
}

void SBSegment::rawClientAddr( struct sockaddr_in target )
{
	_ekp2pBlock._rawClientAddr = target; //copy
}






void SBSegment::importFromEKP2PHeader( std::shared_ptr<ekp2p::EKP2PMessageHeader> fromHeader )
{
	this->sourceKNodeAddr(fromHeader->sourceKNodeAddr() );
	this->relayKNodeAddrVector( fromHeader->relayKNodeAddrVector() );
	this->protocol( fromHeader->protocol() );
	this->rpcType( fromHeader->rpcType() );
}









StreamBuffer::StreamBuffer()
{
	_sb._capacity = DEFAULT_STREAM_BUFFER_CAPACITY;
}






void StreamBuffer::enqueue( std::unique_ptr<SBSegment> target , size_t timeout )
{
	// ロックの獲得
	std::unique_lock<std::mutex> lock(_mtx);  // 他が使用中だとブロッキングする. 他プロセスが解放するとそこからスタート

	if( timeout > 0 ) // タイムアウト付き呼び出しだった場合
	{
		_pushContributer._pushCV.wait_for( lock , std::chrono::seconds(timeout) );
		if( _sb._queue.size() < _sb._capacity ) return;
	}

	else
	{
		_pushContributer._pushCV.wait( lock , [&]{
			return (_sb._queue.size() < _sb._capacity);
		}); // 評価される度に第二引数がtrueであることを確認する
	}

	_sb._queue.push_back( std::move(target) );

	_popContributer._popCV.notify_all();

	return;

}






std::unique_ptr<SBSegment> StreamBuffer::dequeue( size_t timeout )
{
	std::unique_lock<std::mutex> lock(_mtx);

	if( timeout > 0 )
	{
		_popContributer._popCV.wait_for( lock , std::chrono::seconds(timeout) );
		if( _sb._queue.empty() ) return nullptr;
	}

	else
	{
		_popContributer._popCV.wait( lock , [&]{
			return !(_sb._queue.empty()); // emptyでなければ待機状態を解除する
		});
	}


	std::unique_ptr<SBSegment> ret = std::move( _sb._queue.front() );

	_sb._queue.erase( _sb._queue.cbegin() );

	_pushContributer._pushCV.notify_all();

	return std::move( ret );
}





/*
	- 入出力がネックになったらSBを増やす
	- 入出力が詰まってきたらコンシューマーを増やす
*/
