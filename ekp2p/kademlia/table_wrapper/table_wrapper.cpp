#include "table_wrapper.h"

// #include "../k_routing_table.h"
#include "../../../shared_components/middle_buffer/middle_buffer.h"
#include "../../network/inband/k_tag_pack.h"
#include "../k_tag.h"

//#include "../k_protocol_handler/FIND_NODE.cpp"
//#include "../k_protocol_handler/PING.cpp"
//#include "../k_protocol_handler/PONG.cpp"

namespace ekp2p{


/*
TableWrapper::TableWrapper( unsigned int bufferSize ,KRoutingTable *hostTable ){

	_hostTable = hostTable;
	_monitorBuffer = new MiddleBuffer( bufferSize );

	return;
}


TableWrapper::~TableWrapper(){
	delete _monitorBuffer;
}





void TableWrapper::start()
{
	std::thread wrapperThread( [&](){

		std::cout << "wrapper thread started" << "\n";

		unsigned char* segment;
		unsigned int segmentSize = 0;
		KTagPack *kTagPack = nullptr;
		//SocketManager *relatedSocketManager = nullptr;

		for(;;)
		{
			_monitorBuffer->popOne( &segment , &segmentSize, true ); // blocking mode
			// ここで取得されるKTagはRaw状態なのでstructedに変換する必要がある

			KTagPack *kTagPack = new KTagPack;
			kTagPack->importRaw( segment , segmentSize );

			KTag *kTag = new KTag( kTagPack->rawKTag() , kTagPack->rawKTagSize() ); // あまりよくない方法

			autoKTagHandler( kTag, kTagPack->relatedSocketManager() );

			delete kTag;
		}

	});
}
*/





void TableWrapper::stop()
{
				/*
	if( _wrapperThread.joinable() )	{
		_wrapperThread.join();
		return true;
	}
	*/

}




MiddleBuffer *TableWrapper::monitorBuffer()
{
	return _monitorBuffer;
}





/*
bool TableWrapper::autoKTagHandler( void* rawKTag , unsigned int kTagSize , SocketManager* activeSocketManager )
{

	KTag* kTag = new KTag( rawKTag, kTagSize );

	return autoKTagHandler( kTag, activeSocketManager );
}
*/




/*
bool TableWrapper::autoKTagHandler( KTag* kTag, SocketManager* activeSocketManager )
{
	switch( kTag->protocol() ){

		case static_cast<int>(KADEMLIA_PROTOCOL::NORM): // case of NORM			
			_hostTable->update( kTag->_kAddrList[0]->toNode( activeSocketManager ) );
			break;
			
		case static_cast<int>(KADEMLIA_PROTOCOL::PING): // case of PING
			_hostTable->update( kTag->_kAddrList[0]->toNode( activeSocketManager) );
			//kTag->_kAddrList[0]->toNode( activeSocketManager )->SendPONG();
			break;
		
		case static_cast<int>(KADEMLIA_PROTOCOL::PONG): // case of PONG
			//PONGHandler( kTag->_kAddrList[0]->toNode( activeSocketManager ) ); // updateも内部で行われる
			break;

		case static_cast<int>(KADEMLIA_PROTOCOL::FIND_NODE): // case of FIND_NODE
			//FIND_NODEHandler( kTag->_kAddrList[0]->toNode( activeSocketManager) , 10 );
			break;

		default: // case of erro or undefined
			break;
			// error or not defined
			
	}

	delete kTag;

	return false;
};
*/



}; // close ekp2p namespace
