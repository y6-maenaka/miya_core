#include "message_receiver.h"

#include "../network/header/header.h"
#include "../network/socket_manager/socket_manager.h"
#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"



namespace ekp2p
{










void MessageReceiver::start()
{

	std::cout << "MessageReceiver Monitor Started" << "\n";

	unsigned char receiveBuffer[UINT16_MAX]; // UDPのMAXサイズ
	EKP2PMessageHeader header;
	size_t receivedLength;

	for(;;)
	{
		memset( receiveBuffer , 0x00 , UINT16_MAX );
		receivedLength = recvfrom( _socketManager->sock() ,receiveBuffer , UINT16_MAX, 0 , nullptr , 0 ); // 受信

		memcpy( &(header._meta) , receiveBuffer , sizeof(header._meta) ); // meta情報の抽出	
		header.importRaw( receiveBuffer , header._meta._headerLength );
		if( !(header.validate()) ) continue; // 不正なメッセージの場合そのメッセージは破棄する


		/* 上層のモジュールに転送する */
		SBSegment *sbSegment = new SBSegment( receiveBuffer + header.headerLength() , header.payloadLength() );
	
		try{
			_sbHub.at( (header.protocol()) )->pushOne( std::make_unique<SBSegment>( receiveBuffer + header.headerLength(), header.payloadLength()) );
		}
		catch( const std::out_of_range& e ){ // 受信したメッセージプロトコルが規定の範囲を超えた場合
			std::cout << "received 20 over ptorocol message" << "\n";
			continue;
		}catch( const std::exception& e ){ // arrayから取得してきたnullptrオブジェクトに対して何らかのメソッドを呼び出した場合
			std::cout << "received unexpected protocol message" << "\n";
			continue;
		}
	}
		//StreamBufferContainer sbaa;
}







void MessageReceiver::setDestinationStreamBuffer( std::shared_ptr<StreamBufferContainer> target , unsigned short destination )
{
	printf("StreamBuffer seted with -> %p\n", target.get() );
	_sbHub.at(destination) = target;
}







unsigned int MessageReceiver::payload( void *rawEKP2PMSG ,unsigned char** ret )
{
	EKP2PMessageHeader header;
	memcpy( &(header._meta) , rawEKP2PMSG , sizeof(header._meta) );

	bool flag = header.validate();

	std::cout << "+++++++++++++" << "\n";
	std::cout << header.headerLength() << "\n";
	std::cout << header.payloadLength() << "\n";

	for( int i=0; i<header.payloadLength() + header.headerLength(); i++ )
	{
		printf("%02X", static_cast<unsigned char*>(rawEKP2PMSG)[i] );
	}std::cout << "\n";
	std::cout << "+++++++++++++" << "\n";

	std::cout << "flag " << flag << "\n";
	if( flag )
	{
		if( header.payloadLength() <= 0 ) return 0;

		*ret = new unsigned char[ header.payloadLength() ];
		std::cout << "header length -> " << header.headerLength() << "\n";
		std::cout << "payload length -> " << header.payloadLength() << "\n";
		memcpy( *ret , static_cast<unsigned char*>(rawEKP2PMSG) + header.headerLength(), header.payloadLength() );
		
		return header.payloadLength();
	}

	return 0;
};




MessageReceiver::MessageReceiver( SocketManager* target )
{
	_socketManager = target;
}




};
