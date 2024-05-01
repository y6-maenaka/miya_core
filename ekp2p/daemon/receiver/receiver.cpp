#include "receiver.h"


#include "../../network/message/message.hpp"
#include "../../network/header/header.h"

#include "../../network/socket_manager/socket_manager.h"
#include "../../../share/stream_buffer/stream_buffer.h"
#include "../../../share/stream_buffer/stream_buffer_container.h"

#include "../../ekp2p.h"
#include "../../protocol.h"
// #include "./allowed_protocol_set.h"




namespace ekp2p
{




EKP2PReceiver::EKP2PReceiver( std::shared_ptr<SocketManager> target , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
	// _socketManager = target;
	_hostSocketManager = target;
	_toBrokerSBC = toBrokerSBC;

	std::cout << "EKP2P::daemon::Receiver just initialized" << "\n";
}








int EKP2PReceiver::start()
{
	if( _hostSocketManager == nullptr ) return -1;
	if( _toRoutingTableUpdatorSBC == nullptr ) return -1;
	if( _toBrokerSBC == nullptr ) return -1;

	std::thread ekp2pReceiver([&]()
	{
		std::cout << "\x1b[32m" << "EKP2P::daemon::Receiver ekp2pReceiver thread started" << "\x1b[39m" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() ); 

		size_t receivedLength = 0; 
		std::shared_ptr<unsigned char> rawMessage;
		std::shared_ptr<EKP2PMessage> message;
		std::shared_ptr<EKP2PMessageHeader> header;
		struct sockaddr_in fromAddr;
		
		for(;;)
		{
			receivedLength = _hostSocketManager->receive( &rawMessage, fromAddr );
			std::cout << "[notify] Message Received" << "\n";

			if( receivedLength <= 0 ) continue;
			// receivedLength = recvfrom( _listeningSocketManager->sock() , receiveBuffer.get() , UINT16_MAX , 0 , nullptr , 0 );

			if( (message = parseRawEKP2PMessage( rawMessage , receivedLength )) == nullptr )
			{
				std::cout << "不正なデータセグメントを受信" << "\n";
				continue;
			}
			header = message->header();

			int protocol = static_cast<int>( header->protocol() );
			std::unique_ptr<SBSegment> sb = std::make_unique<SBSegment>();
			sb->importFromEKP2PHeader( header );
			sb->forwardingSBCID( header->protocol() );
			sb->body( message->payload() , header->payloadLength() );
			sb->rawSenderAddr( fromAddr );
	
			_toBrokerSBC->pushOne( std::move(sb) );
		}
		return;

	});

	ekp2pReceiver.detach();
	std::cout << "ekp2pReceiver thread detached" << "\n";

	return 0;
}






void EKP2PReceiver::toRoutingTableUpdatorSBC( std::shared_ptr<StreamBufferContainer> sbc )
{
	_toRoutingTableUpdatorSBC = sbc;
}








std::shared_ptr<EKP2PMessage> EKP2PReceiver::parseRawEKP2PMessage( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{

	std::shared_ptr<EKP2PMessage> ret = std::make_shared<EKP2PMessage>();

	// ヘッダーのインポート
	ret->header()->importRawSequentially( fromRaw );

	// 簡易的な検証を数項目行う
	if( memcmp( ret->header()->token() , "MIYA" , 4 ) != 0 ) return nullptr; // トークン不一致

	// ペイロードのインポート 二重で領域確保する為効率が悪い
	std::shared_ptr<unsigned char> payload = std::shared_ptr<unsigned char>( new unsigned char[ret->header()->payloadLength()] );
	memcpy( payload.get() , fromRaw.get() + ret->header()->headerLength(), ret->header()->payloadLength() );

	//std::shared_ptr<unsigned char> payloadHead = std::shared_ptr<unsigned char>( fromRaw.get() + ret->header()->headerLength() );
	//std::shared_ptr<unsigned char> payloadHead = std::make_shared<unsigned char>( *(fromRaw.get() + ret->header()->headerLength()) );
	ret->payload( payload );

	return ret;
	return nullptr;
}





};



