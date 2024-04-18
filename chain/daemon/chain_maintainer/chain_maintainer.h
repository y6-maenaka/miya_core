#ifndef B798C929_E8EC_4B54_930F_9DC90657E0C5
#define B798C929_E8EC_4B54_930F_9DC90657E0C5



#include <iostream>
#include <memory>
#include <thread>
#include <any>
#include <vector>


struct SBSegment;
class StreamBufferContainer;


namespace block
{
	class BlockCIterator;
	struct BlockHeader;
	struct Block;
};






namespace chain
{


struct MiyaChainState;
class LightUTXOSet;
class BlockLocalStrageManager;










class ChainMaintainer
{
private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<MiyaChainState> _chainState;

	std::shared_ptr<LightUTXOSet> _utxoSet;
	std::shared_ptr<BlockLocalStrageManager> _localStrageManager;


public:
	ChainMaintainer( std::shared_ptr<StreamBufferContainer> incomingSBC , 
									 std::shared_ptr<MiyaChainState> chainState, // 先頭ブロック取得用
									 std::shared_ptr<LightUTXOSet> utxoSet, // トランザクション保存用
									 std::shared_ptr<BlockLocalStrageManager> localStrageManager // ブロック保存用
									);
	int start();
  
	void accidentFork(); // 偶発的に発生するフォーク
};



/*


	 チェーン維持の管理事項
	
	 受信したBlockがlocalChainHeadより高い
	  -> 数個先( 検証して辿って取り寄せマージ)
		-> 一個先( 検証して取り込み & ブロードキャスト ) & マイニングプロセスへの中止命令


		マイニングをスタートするには( localChainHeadをマイニングプロセスへ渡す )



*/







};





#endif // B798C929_E8EC_4B54_930F_9DC90657E0C5

