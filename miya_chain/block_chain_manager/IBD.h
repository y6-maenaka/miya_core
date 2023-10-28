#ifndef AD809715_E27B_49D1_BAD1_616A5192280B
#define AD809715_E27B_49D1_BAD1_616A5192280B



#include <memory>
#include <vector>
#include <any>




struct SBSegment;
class StreamBufferContainer;


namespace block
{

struct BlockHeader;

};




namespace miya_chain
{



class MiyaChainState;


struct IBDBCB // (Initial Block Donwload Block Control Block)
{
	/* 
	  -ブロックヘッダを順番に並べる 
		- ブロックのストア状況を保存する
			
	 */

	// std::shared_ptr<unsigned char> blockHash;
	unsigned char _blockHash[32];
	std::shared_ptr<block::BlockHeader> _blockHeader;
	uint32_t _height; // いらないかも
	int status; 
	// 1 : blockHashまで受信完了
	// 2 : ヘッダーまで受信完了
	// 3 : トランザクション収集中
	// 4 : 保存済み
	// -1 : エラー発生

};







class IBDManager
{

private:
	std::vector<IBDBCB> _IBDBCBVector;


public:
	bool start( std::shared_ptr<MiyaChainState> chainState , std::shared_ptr<StreamBufferContainer> toRequesterSBC );

};



/*
 初回のブロックヘッダーダウンロードはメモリ上で行う
*/
};



#endif // 

