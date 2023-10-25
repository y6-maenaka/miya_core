#include "request_handler.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "./stun_message.h"
#include "../../ekp2p.h"
#include "../../daemon/sender/sender.h"


namespace ekp2p
{




StunRequestHandlerDaemon::StunRequestHandlerDaemon( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
    _incomingSBC = incomingSBC;
    _toBrokerSBC = toBrokerSBC;
}




int StunRequestHandlerDaemon::start()
{
	std::cout << "--- 1 ---" << "\n";

    if( _incomingSBC == nullptr ) return -1;

		std::cout << "--- 2 --- " << "\n";
    if( _toBrokerSBC == nullptr ) return -1;

		std::cout << "passed here" << "\n";

    std::thread stunRequestHandler([&]()
    {
        std::cout << "StunRequestHandler started" << "\n";

        struct StunResponse response;
        std::unique_ptr<SBSegment> popedSB;
        std::shared_ptr<unsigned char> rawStunResponse; size_t rawStunResponseLength;
        for(;;)
        {
	    std::cout << "pop befoer" << "\n";
            popedSB = _incomingSBC->popOne();
	    std::cout << "StunRequest Received" << "\n";

            response.sockaddr_in( popedSB->rawSenderAddr() );
            rawStunResponseLength = response.exportRaw( &rawStunResponse );

	    std::cout << "..........................................." << "\n";
	    std::cout << "response rawClientAddr()" << "\n";
	    std::cout << inet_ntoa(response.toSockaddr_in().sin_addr) << "\n";
	    std::cout << ntohs( response.toSockaddr_in().sin_port) << "\n";
	    std::cout << "raw "<< rawStunResponseLength  << " :: ";
	    for( int i=0; i<rawStunResponseLength; i++){
		printf("%02X", rawStunResponse.get()[i]);
	    } std::cout << "\n";
	    std::cout << "..........................................." << "\n";


        popedSB->body( rawStunResponse, rawStunResponseLength );
	    popedSB->protocol( DEFAULT_DAEMON_FORWARDING_SBC_ID_NATER  );
        popedSB->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER ); // 転送先の設定
        popedSB->sendFlag( EKP2P_SENDBACK | EKP2P_SEND_UNICAST ); // 送信モードの設定
        _toBrokerSBC->pushOne( std::move(popedSB) );

	    std::cout << "StunResponse Sended" << "\n";
        }

    });
    stunRequestHandler.detach();
    std::cout << "StunRequestHandler detached" << "\n";

    return 0;
}


}; // close ekp2p namespace
