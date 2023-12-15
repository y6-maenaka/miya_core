#ifndef FA2712E0_8314_41BD_818A_32C8A01AC25F
#define FA2712E0_8314_41BD_818A_32C8A01AC25F


#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <cassert>
#include <chrono>
#include <iostream>

#include <unistd.h>

namespace block
{
struct Block;
struct BlockHeader;
};

class StreamBufferContainer;
struct SBSegment;




namespace miya_chain
{


class BDFilter;
class BDVirtualChain;
class LightUTXOSet;
class BlockLocalStrageManager;



constexpr unsigned int DEFAULT_BLOCK_HEADER_DOWNLOAD_AGENT_COUNT = 1;
constexpr unsigned int DEFAULT_BLOCK_DOWNLOAD_AGENT_COUNT = 3; // ブロックダウンロードエージェントの起動数
constexpr unsigned int DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE = 10; // 一つのブロックダウンロードエージェントが一回で担当するブロック数
constexpr unsigned int DEFAULT_BD_MAX_TIMEOUT_COUNT = 4;

// first: blockHash , second: ブロック管理ブロック
using VirtualMiyaChain = std::vector< std::pair< std::shared_ptr<unsigned char>, std::shared_ptr<struct BDBCB>> >;




enum class BDState : int
{
	BlockHeaderNotfound,
	BlockNotfound,
	BlockHeaderReceived = 2, // ブロックヘッダの取得完了
	BlockHeaderValidated, // ブロックヘッダの検証完了
	BlockBodyReceived, // ブロック本体の取得完了
	BlockBodyValidated,  // ブロックの本体検証完了
	BlockStored, // ブロック本体の取得がが完了し,ローカルに保存済み

	NowDownloading = -1, // ダウンロード中
  	Error = -2, // なんらかのエラー発生
	Empty = 0,
	None = -3
};





struct BDBCB // ( Block Donwload Block Control Block)
{
	std::shared_ptr<block::Block> block;
	uint32_t height; // いらないかも
	int status;

	BDBCB();
	void header( std::shared_ptr<block::BlockHeader> target ); // ヘッダーだけ挿入するケース

  // 間接Getterメソッド
	std::shared_ptr<unsigned char> blockHash() const;
	std::shared_ptr<unsigned char> prevBlockHash() const;

	void print();
};








// このメソッドに対してaddするのは,MiyaChainMSGから取り出された生のヘッダまたはブロックのみ
class BDVirtualChain : public std::enable_shared_from_this<BDVirtualChain>
{
private:
  std::shared_ptr<BDFilter> _bdFilter;
  std::shared_ptr<LightUTXOSet> _utxoSet;
	std::shared_ptr<BlockLocalStrageManager> _localStrageManager;

  VirtualMiyaChain _chainVector;// チェーン順に並んだBDリスト
  
	struct IncompleteBlockVector
   {
		std::shared_mutex _mtx;
		std::condition_variable _cv;

		std::vector< VirtualMiyaChain::iterator > _undownloadBlockVector; // チェーンに繋がれたがブロック本体のダウンロードが完了していない要素イテレータのベクター
		VirtualMiyaChain::iterator _validateBlockHead;

		void push( VirtualMiyaChain::iterator target );
		std::vector< VirtualMiyaChain::iterator > pop( size_t size = DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE );
		size_t size();
		VirtualMiyaChain::iterator popUnvalidateBlockHead();
  } _incompleteBlockVector;


	std::shared_ptr<unsigned char> _currentStartHash;
	std::shared_ptr<unsigned char> _stopHash;

	std::shared_mutex _mtx;
	std::condition_variable_any _cv; // 多少のオーバーヘッドを伴うらしい

protected:

	std::shared_ptr<unsigned char> chainHeadHash();
	const std::pair< std::shared_ptr<unsigned char> , std::shared_ptr<struct BDBCB> > chainHead(); // virtualChain最後尾を取得
  VirtualMiyaChain chainVector();

	// 仮想チェーン構築系メソッド
	// めっそ度変数の_currentStartHash(startBlockHash)から_stopHashまでのブロックヘッダを収集する あくまで集めるだけのメソッド, 検証はしない(Filterで自動的に行う)
	void blockHeaderDownloader( std::shared_ptr<StreamBufferContainer> toRequesterSBC );
	void blockDownloader( std::shared_ptr<StreamBufferContainer> toRequesterSBC ); // ブロックヘッダよりブロック本体をダウンロードする処理

	
	bool mergeToLocalChain(); // 仮想チェーンの全てをローカルの本番チェーンにマージする // 並列保存しない場合

public:
  BDVirtualChain(   // 自身チェーンとの分岐点,目標ハッシュ(nullは最先端)をセットする
									std::shared_ptr<LightUTXOSet> utxoSet , 
									std::shared_ptr<BlockLocalStrageManager> localStrageManager ,
									std::shared_ptr<block::Block> startBlock ,
									std::shared_ptr<unsigned char> stopHash = nullptr 
									);

	BDVirtualChain( // 自身チェーンとの分岐点,最終譚までのブロックヘッダをセットする
									std::shared_ptr<LightUTXOSet> utxoSet,
									std::shared_ptr<BlockLocalStrageManager> localStrageManager,
									std::shared_ptr<block::Block> startBlock,
									std::vector< std::shared_ptr<block::BlockHeader>> _headerVector // チェーンの順に並んでいること
									);

	~BDVirtualChain();

  void requestedExtendChain( std::function<struct BDBCB( std::shared_ptr<unsigned char> )> findPopCallback ); // 仮想チェーンの最先端ブロックハッシュを元にポップする

	// 何のケースでもブロックヘッダを前回種しない限りはブロックダウンロード、検証、ストアは行われない	
	bool startSequentialAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC ); // 主にIBD　ローカルファイルに書き込みながらチェーンを組み立てる
	bool startParallelAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC ); // 主にマージ ローカルファイルには保存せずブロックをすべてダウンロードする

	// bool add( std::shared_ptr<block::BlockHeader> header , int defaultStatus = static_cast<int>(BDState::None) );
	bool add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector , int defaultStatus = static_cast<int>(BDState::None) );
	bool add( std::vector<std::shared_ptr<block::Block>> blockVector, int defaultStatus = static_cast<int>(BDState::None) );

	size_t chainLength();
	void printVirtualChain();
	void printFilter();
	void printHeaderValidationPendingQueue();
	void printMergePendingQueue();
};




};



#endif
