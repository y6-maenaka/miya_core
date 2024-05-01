#include "block_chain_manager.h"

namespace chain
{

BlockChainManager::BlockChainManager( std::shared_ptr<HeaderStore> headerStore , std::shared_ptr<TransactionStore> txStore )
{
	_store._headerStore = headerStore;
	_store._txStore = txStore;
}


bool BlockChainManager::syncBlockHeader()
{
	// 最先端のブロックを取得してから自身のローカルチェーンの最先端に追いついたら同期を終了する

}



bool BlockChainManager::syncBlock()
{
}


bool BlockChainManager::InitialiBlockDownload()
{

	// 1. 接続しているノードから最先端のブロックヘッダを取得する
	std::unique_ptr<SBSegment> requestSB = std::make_unique<SBSegment>();
	_toSenderSB->pushOne( std::move(requestSB) );

}


};
