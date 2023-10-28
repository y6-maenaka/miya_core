#include "./IBD.h"

#include "../message/message.h"
#include "../message/command/command_set.h"

#include "../miya_core_manager.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../ekp2p/daemon/sender/sender.h"


namespace miya_chain
{




bool IBDManager::start( const std::shared_ptr<MiyaChainState> chainState , std::shared_ptr<StreamBufferContainer> toRequesterSBC )
{

	// IDB
	// 1. 全ヘッダの収集 (ヘッダ情報はキャッシュする)
	// 2. 対応するブロックの収集


	if( chainState == nullptr ) return false;
	if( toRequesterSBC == nullptr ) return false;

	// リクエストメッセージの作成
	MiyaChainMSG_GETBLOCKS getbloksMSG;
	getbloksMSG.startHash( chainState->chainHead() );
	MiyaChainCommand command;

	// ヘッダ収集終了のトリガーは  getdataに対しての応答がnotfoundだった場合
	for(;;)
	{
		std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
		command = getbloksMSG; // 展開用に一旦パックする
		requestSB->options.option1 = command;
		requestSB->options.option2 = MiyaChainMSG_GETBLOCKS::command;
		requestSB->sendFlag( ekp2p::EKP2P_SEND_BROADCAST );

		(std::get<MiyaChainMSG_GETBLOCKS>(std::any_cast<MiyaChainCommand>(requestSB->options.option1))).print();

		toRequesterSBC->pushOne( std::move(requestSB) );

		break;
	}

	std::cout << "IBDManager 終了" << "\n";


	return false;
}



};
