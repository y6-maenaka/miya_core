#include "chain_maintainer.h"


#include "../../../shared_components/stream_buffer/stream_buffer.h"
#include "../../../shared_components/stream_buffer/stream_buffer_container.h"

#include "../../message/message.h"
#include "../../message/command/command_set.h"

#include "../../block/block.h"
#include "../../transaction/coinbase/coinbase.h"

#include "../../miya_chain_manager.h"

#include "../../virtual_chain/virtual_chain.h"
#include "../../block/block_c_iterator.h"


namespace miya_chain
{




ChainMaintainer::ChainMaintainer( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<MiyaChainState> chainState, std::shared_ptr<LightUTXOSet> utxoSet, std::shared_ptr<BlockLocalStrageManager> localStrageManager )
{
	_incomingSBC = incomingSBC;
	_chainState = chainState;
	_utxoSet = utxoSet;
	_localStrageManager = localStrageManager;
}
	






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

		std::shared_ptr<block::BlockHeader> header(){
			return this->block->header();
		}

	} ;

	
	std::shared_ptr<block::Block> currentChainHeadBlcok;
	std::thread miyaChainMaintainer([&]()
	{
	
		for(;;)
		{
			popedSB = _incomingSBC->popOne();
			MiyaChainMessage msg = std::any_cast<MiyaChainMessage>( popedSB->options.option1 );

			switch( msg.commandIndex() )
			{
				case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK):
				{
					ReceivedBlock receivedBlock{ std::get<MiyaChainMSG_BLOCK>(msg.payload()).block() };
				}

				case static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_HEADERS):
				{
				}

			}


			if( msg.commandIndex() !=  static_cast<int>(MiyaChainCommandIndex::MiyaChainMSG_BLOCK) ) continue; // BLOCKメッセージ以外は破棄する
	
			ReceivedBlock receivedBlock{ std::get<MiyaChainMSG_BLOCK>(msg.payload()).block() };
		
			// この条件一致で処理が異なる
			if( receivedBlock.height <= _chainState->height() ) // receivedBlockHeightがlocalHeadHeight以下であれば自身のチェーンを優先し，無視する
				continue;


			// 自身のチェーンヘッドより高いブロックを受信したら,取り込みシーケンスに入る	
			// BDVirtualChain virtualChain( currentChainHeadBlcok , );

	
			// block::BlockCIterator bciterator( _localStrageManager , _chainState->chainHead() );
			
	
			// 自身のチェーンにマージする処理
			// 受信したブロックを検証して,自身のチェーン先頭にマージできるようにする		
			//  -> 自身チェーンの高さと収集したブロックの高さが一致し,尚且つ内容が一致するまで収集を続ける
	
		

			currentChainHeadBlcok; // ブロックを更新する
		}


	});


	return 0;
}
/*
	基本的にChainMaintainerに流入していくるSBSegmentは自身で発掘したブロック もしくは 外部からのブロック
	// ブロック先頭が変更になると,マイニングモジュールに通知し,マイニング対象を変更する
*/












};
