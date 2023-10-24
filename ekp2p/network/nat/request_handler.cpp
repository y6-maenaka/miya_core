#include "request_handler.h"

#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "./stun_message.h"
#include "../../ekp2p.h"
#include "../../daemon/sender/sender.h"


namespace ekp2p
{



StunRequestHandlerDaemon::StunRequestHandlerDaemon( std::shared_ptr<StreamBufferContainer> incomingSBC ,std::shared_ptr<StreamBufferContainer> toBrokerSBC )
{
    _incomingSBC = incomingSBC;
    _toBrokerSBC = toBrokerSBC;
}




int StunRequestHandlerDaemon::start()
{
    if( _incomingSBC == nullptr ) return -1;
    if( _toBrokerSBC == nullptr ) return -1;

    std::thread stunRequestHandler([&]()
    {
        std::cout << "StunRequestHandler started" << "\n";

        struct StunResponse response;
        std::unique_ptr<SBSegment> popedSB;
        std::shared_ptr<unsigned char> rawStunResponse; size_t rawStunResponseLength;
        for(;;)
        {
            popedSB = _incomingSBC->popOne();

            response.sockaddr_in( popedSB->rawClientAddr() );

            rawStunResponseLength = response.exportRaw( &rawStunResponse );

            popedSB->body( rawStunResponse, rawStunResponseLength );
            popedSB->forwardingSBCID( DEFAULT_DAEMON_FORWARDING_SBC_ID_SENDER ); // 転送先の設定
            popedSB->sendFlag( EKP2P_SENDBACK | EKP2P_SEND_UNICAST ); // 送信モードの設定
            _toBrokerSBC->pushOne( std::move(popedSB) );
        }

    });
    stunRequestHandler.detach();
    std::cout << "StunRequestHandler detached" << "\n";

    return 0;
}


}; // close ekp2p namespace
