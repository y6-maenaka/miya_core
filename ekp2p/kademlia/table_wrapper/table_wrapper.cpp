#include "table_wrapper.h"

#include "../"

namespace ekp2p{


TableWrapper::TableWrapper( unsigned int bufferSize ,KRoutingTable *hostTable ){

	_hostTable = hostTable;
	_middleBuffer = new MiddleBuffer( bufferSize );

	return;
}


TableWrapper::~TableWrapper(){
	delete _middleBuffer;
}





void TablerWrapper::startThread()
{
	_wrapperThread = std::thread( [&]()
	{
		std::cout << "hello" << "\n";

		// _midBuffer.pop();
		
	});

}





bool TableWrapper::joinThread()
{
	if( _wrapperThread.joinable() )	{
		_wrapperThread.join();
		return true;
	}

	return false;
}




MiddleBuffer *middleBuffer()
{
	return _inbandMidBuffer;
}






bool TableWrapper::autoKTagHandler( void* rawKTag , unsigned int kTagSize , SocketManager* activeSocketManager )
{

	KTag* kTag = new KTag( rawKTag, kTagSize );

	return autoKTagHandler( kTag, activeSocketManager );
}





bool TableWrapper::autoKTagHandler( KTag* kTag, SocketManager* activeSocketManager )
{
	switch( kTag->protocol() ){

		case static_cast<int>(KADEMLIA_PROTOCOL::NORM): // case of NORM			
			update( kTag->_kAddrList[0]->toNode( activeSocketManager ) );
			break;
			
		case static_cast<int>(KADEMLIA_PROTOCOL::PING): // case of PING
			update( kTag->_kAddrList[0]->toNode( activeSocketManager) );
			kTag->_kAddrList[0]->toNode( activeSocketManager )->SendPONG();
			break;
		
		case static_cast<int>(KADEMLIA_PROTOCOL::PONG): // case of PONG
			PONGHandler( kTag->_kAddrList[0]->toNode( activeSocketManager ) ); // updateも内部で行われる
			break;

		case static_cast<int>(KADEMLIA_PROTOCOL::FIND_NODE): // case of FIND_NODE
			FIND_NODEHandler( kTag->_kAddrList[0]->toNode( activeSocketManager) , 10 );
			break;

		default: // case of erro or undefined
			break;
			// error or not defined
			
	}

	delete kTag;

	return false;
};




}; // close ekp2p namespace
