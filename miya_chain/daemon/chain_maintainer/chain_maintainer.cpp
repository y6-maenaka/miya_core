#include "chain_maintainer.h"


#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"
#include "../../message/command/command_set.h"

#include "../../block/block.h"
#include "../../transaction/coinbase/coinbase.h"

#include "../../miya_chain_manager.h"


namespace miya_chain
{











int ChainMaintainer::start()
{

	
	std::unique_ptr<SBSegment> popedSB;

	struct ReceivedBlock
	{
		std::shared_ptr<block::Block> block;
		int height;
		
		ReceivedBlock( std::shared_ptr<block::Block> target ){
			block = target;
			height = target->coinbase()->height();
		};

	} ;


	std::thread miyaChainMaintainer([&]()
	{
	
		for(;;)
		{
			popedSB = _incomingSBC->popOne();

			MiyaChainMessage msg = std::any_cast<MiyaChainMessage>( popedSB->options.option1 );
			if( msg.commandIndex() !=  static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK) ) continue; // BLOCKメッセージ以外は破棄する
	
			ReceivedBlock receivedBlock{ std::get<MiyaChainMSG_BLOCK>(msg.payload()).block() };
		
			// この条件一致で処理が異なる
			if( receivedBlock.height <= _chainState->height() ) // receivedBlockHeightがlocalHeadHeight以下であれば自身のチェーンを優先し，無視する
				continue;


			// 自身チェーンと一致する箇所までヘッダを収集する必要がある :: その間の更新操作はどうするか



			// 自身のチェーンにマージする処理
			// 受信したブロックを検証して,自身のチェーン先頭にマージできるようにする		
			//  -> 自身チェーンの高さと収集したブロックの高さが一致し,尚且つ内容が一致するまで収集を続ける


		}


	});


	return 0;
}









};
