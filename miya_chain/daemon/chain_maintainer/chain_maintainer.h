#ifndef B798C929_E8EC_4B54_930F_9DC90657E0C5
#define B798C929_E8EC_4B54_930F_9DC90657E0C5



#include <iostream>
#include <memory>
#include <thread>
#include <any>


struct SBSegment;
class StreamBufferContainer;









namespace miya_chain
{


struct MiyaChainState;






class ChainMaintainer
{
private:
	std::shared_ptr<StreamBufferContainer> _incomingSBC;
	std::shared_ptr<MiyaChainState> _chainState;


public:
	int start();

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

