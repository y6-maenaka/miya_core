#include "receiver.h"


#include "../../network/message/message.h"
#include "../../network/header/header.h"

#include "../../network/socket_manager/socket_manager.h"
#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"


#include "../../protocol.h"
// #include "./allowed_protocol_set.h"




namespace ekp2p
{




Receiver::Receiver( std::shared_ptr<SocketManager> target )
{
	// _socketManager = target;
	_listeningSocketManager = target;

	std::cout << "EKP2P::daemon::Receiver just initialized" << "\n";
}








void Receiver::start()
{





	/*
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


		// 上層のモジュールに転送する 
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
	*/

	/*
	std::thread ekp2pReceiver([&]()
	{
		
	});
	*/

	std::thread ekp2pReceiver([&]()
	{
		std::cout << "EKP2P::daemon::Receiver ekp2pReceiver thread started" << "\n";

		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 

		size_t receivedLength = 0; 
		std::shared_ptr<unsigned char> receiveBuffer = std::shared_ptr<unsigned char>( new unsigned char[UINT16_MAX] ); // マックスサイズでもたかが知れているので最大で
		memset( receiveBuffer.get() , 0x00 , sizeof(UINT16_MAX) );

		std::shared_ptr<EKP2PMessage> message;
		std::shared_ptr<EKP2PMessageHeader> header;
		
		for(;;)
		{
			std::cout << "<ekp2p::Receiver> check 1" << "\n";
			std::cout << "socket -> " << _listeningSocketManager->sock() << "\n";

			// receivedLength = recvfrom( _listeningSocketManager->sock() , receiveBuffer.get() , 0 ,UINT16_MAX, nullptr, 0 ); // セグメントの受信
			receivedLength = recvfrom( _listeningSocketManager->sock() , receiveBuffer.get() , UINT16_MAX , 0 , nullptr , 0 );


			message = parseRawEKP2PMessage( receiveBuffer, receivedLength ); // rawから構造化
			if( message == nullptr ) {
				std::cout << "不正なデータセグメント" << "\n";
				continue;
			}
			header = message->header();

			std::cout << "<ekp2p::Receiver> check 2" << "\n";

			int protocol = static_cast<int>( header->protocol() );

			std::cout << "<ekp2p::Receiver> check 3" << "\n";

			std::unique_ptr<SBSegment> sb = std::make_unique<SBSegment>();
			sb->sourceKNodeAddr( header->sourceKNodeAddr() );
			sb->relayKNodeAddrVector( header->relayKNodeAddrVector() );
			sb->body( message->payload() , header->payloadLength() );


			std::cout << "-----------------------------------" << "\n";
			std::cout << "protocol :: " << header->protocol() << "\n";
			header->printRaw();
			if( _sbHub.at(protocol) == nullptr ) std::cout << "_sbHub.at(ptorocol) is nullptr" << "\n";
			if( _allowedProtocolSet[protocol] == false ) std::cout << "protocol :: " <<  protocol << " :: is not allowed" << "\n";
			std::cout << "-----------------------------------" << "\n";


			if( _sbHub.at(protocol) == nullptr || !(_allowedProtocolSet[protocol]) ){
				std::cout << "Error Pack Received" << "\n";
				continue;
			}

			std::cout << "<ekp2p::Receiver> check 5" << "\n";
			_sbHub.at(protocol)->pushOne( std::move(sb) ); // 受信セグメントの転送
		}
		return;

	});

	ekp2pReceiver.detach();
	std::cout << "ekp2pReceiver thread detached" << "\n";

}





int Receiver::forwardingDestination( std::shared_ptr<StreamBufferContainer> sb , unsigned short destination )
{
	if( destination >= MAX_PROTOCOL ) return -1;
	if( sb == nullptr ) return -1;

	std::cout << "Set forwarding destination with :: " << destination << "\n";
	_sbHub.at(destination) = sb;

	return destination;
}





/*
unsigned int Receiver::payload( void *rawEKP2PMSG ,unsigned char** ret )
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
*/







std::shared_ptr<EKP2PMessage> Receiver::parseRawEKP2PMessage( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{

	std::shared_ptr<EKP2PMessage> ret = std::make_shared<EKP2PMessage>();

	// ヘッダーのインポート
	ret->header()->importRawSequentially( fromRaw );

	// 簡易的な検証を数項目行う
	if( std::memcmp( ret->header()->token() , "MIYA" , 4 ) != 0 ) return nullptr; // トークン不一致
	if( ret->header()->protocol() >= MAX_PROTOCOL ) return nullptr; // 受け付けていないプロトコル

	// ペイロードのインポート
	std::shared_ptr<unsigned char> payloadHead( fromRaw.get() + ret->header()->headerLength() );
	ret->payload( payloadHead );


	return ret;
	return nullptr;
}





};



