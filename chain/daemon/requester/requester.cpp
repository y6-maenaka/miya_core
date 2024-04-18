#include "requester.h"

#include "../../../share/stream_buffer/stream_buffer.h"
#include "../../../share/stream_buffer/stream_buffer_container.h"

#include <message/message.h>
#include <message/command/command_set.h>

#include "../../../ekp2p/daemon/sender/sender.h"

namespace chain
{



MiyaChainRequester::MiyaChainRequester( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> toMiyaCHainBrokerSBC )
{
	_incomingSBC = incomingSBC;
	_toMiyaChainBrokerSBC = toMiyaCHainBrokerSBC;
}



int MiyaChainRequester::start()
{

	if( _incomingSBC == nullptr ){
		std::cerr << "(MiyaChainRequester) 有効なIncomingSBCがセットされていません" << "\n";
		return -1;
	}
	if( _toMiyaChainBrokerSBC == nullptr ){
		std::cerr << "(MiyaChainRequester) 有効なToMiyaChainBrokerSBCがセットされていません" << "\n";
		return -1;
	}



	printf("(MiyaChainRequester) ToMiyaChainBrokerSBC :: %p\n", _toMiyaChainBrokerSBC.get() );

	std::thread miyaChainRequester([&]()
	{
		std::cout << "MiyaChain::daemon::Requester miyaChainRequester thread started" << "\n";
		_activeSenderThreadIDVector.push_back( std::this_thread::get_id() );

		std::unique_ptr<SBSegment> popedSB;
		// sb::option1 = MiyaCommand
		// sb::option2 = command(名・識別子)

		const char* command;
		MiyaChainCommand commandBody;
		MiyaChainMessage message;

		for(;;)
		{
			popedSB = _incomingSBC->popOne();
			commandBody = std::any_cast<MiyaChainCommand>(popedSB->options.option1); // MiyaCommand::command
			command = std::any_cast<const char*>(popedSB->options.option2); // command
			message.payload( commandBody , command );

			std::shared_ptr<unsigned char> rawMiyaChainMSG; size_t rawMiyaChainMSGLength;
			rawMiyaChainMSGLength = message.exportRaw( &rawMiyaChainMSG );

			std::cout << "Raw MiyaChainMSG(" << rawMiyaChainMSGLength << ") :: ";
			for( int i=0; i<rawMiyaChainMSGLength ; i++){
				printf("%02X", rawMiyaChainMSG.get()[i]);
			} std::cout << "\n";

			popedSB->body( rawMiyaChainMSG , rawMiyaChainMSGLength );
			popedSB->sendFlag( ekp2p::EKP2P_SENDBACK | ekp2p::EKP2P_SEND_UNICAST );


			_toMiyaChainBrokerSBC->pushOne( std::move(popedSB) );

			std::cout << "(MiyaChainRequester) :: Forwarding to Broker" << "\n";
		}
	});
	miyaChainRequester.detach();
	std::cout << "MiyaChainRequester thread detached" << "\n";

	return -1;
}


};
