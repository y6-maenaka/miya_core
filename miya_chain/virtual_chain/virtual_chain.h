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


namespace block
{
struct Block;
struct BlockHeader;
};

class StreamBufferContainer;
struct SBSegment;




namespace miya_chain
{


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
	Empty = 0
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




struct BlockHashAsKey // filterのunordered_mapのキーとして使う
{
	unsigned char _blockHash[32];
	struct Hash;

	BlockHashAsKey(){};
	BlockHashAsKey( unsigned char* target );
	BlockHashAsKey( std::shared_ptr<unsigned char> target ) : BlockHashAsKey( target.get() ){};
	BlockHashAsKey( struct BDBCB fromCB );
	bool operator==(const BlockHashAsKey& bh ) const;
  bool operator!=(const BlockHashAsKey& bh ) const;

	void printHash() const;
};

struct BlockHashAsKey::Hash {
    std::size_t operator()(const BlockHashAsKey& key) const;
};








struct PrevBlockHashAsKey
{
	unsigned char _prevBlockHash[32];
	struct Hash;

	PrevBlockHashAsKey( struct BDBCB fromCB );
	PrevBlockHashAsKey( std::shared_ptr<unsigned char> prevBlockHash );

	bool operator==(const PrevBlockHashAsKey& bh ) const;
  bool operator!=(const PrevBlockHashAsKey& bh ) const;

	std::shared_ptr<unsigned char> prevBlockHash() const;
};

struct PrevBlockHashAsKey::Hash {
    std::size_t operator()(const PrevBlockHashAsKey& key) const;
};






class BDFilter // 先駆けてブロックヘッダを受信するので,それをフィルタとして使う
{
private:

  // ブロックヘッダーのみ受け付ける
  struct Filter // layer1では通過したブロックを記録し,重複してブロックを通過させないようにする
	{
		public:
			std::mutex _mtx;
			std::unordered_map< BlockHashAsKey , std::pair<struct BDBCB, std::shared_ptr<struct BDBCB>> , BlockHashAsKey::Hash > _filterMap; // layer 1(全てのヘッダ)
 			bool _closeing = false; // 現時点でフィルターに登録していない要素は追加しない（破棄する）
			
  } _filter; // ダウンロードしなければならない全てのヘッダ情報
    /* ヘッダの検証スレッドも宣言と同時に起動される */


	class ValidationHeaderQueue // 受信したブロックヘッダが待機させられるバッファ
	{
		public:
			std::vector< std::pair<BlockHashAsKey , struct BDBCB > >  _pendingQueue;

			std::mutex _mtx;
	  		std::condition_variable _cv;

			void push( std::pair<BlockHashAsKey , struct BDBCB> targetPair );
	} _validationHeaderQueue;


  class PendingMergeQueue // ここに検証済みのブロックヘッダが入ってきた時点でブロック本体のダウンロードを開始する
	{
		public:
			// ここでのキーはprevBlockHashになる(virtualChainから検索しやすいように)
			std::unordered_map< PrevBlockHashAsKey , struct BDBCB , PrevBlockHashAsKey::Hash > _pendingMap;

			std::mutex _mtx;
			// first: prevBLockHash , second; bcbdb
			struct BDBCB findPop( std::shared_ptr<unsigned char> prevBlockHash ); // callback for virtual chain(find)
			void push( struct BDBCB cb );
	} _pendingMergeQueue;


	BDVirtualChain* _virtualChain;

public:
    BDFilter( BDVirtualChain* virtualChain ); // ヘッダを検証するスレッドを起動する

		/* いずれもlayer1に対する操作 ユーザプログラムから直接layer2を操作することはない */
		bool add( struct BDBCB targetBCB );
		bool add( std::shared_ptr<block::BlockHeader> header);
		bool add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector );
		bool add( std::shared_ptr<block::Block> block );

		// 仮想チェーンにブロックヘッダが取り込まれ,ブロック本体ダウンロードする際は,フィルタの中間層を飛ばして直接仮想チェーンで管理されているBDBに変更を加える
		void updateBlockPtr( std::shared_ptr<struct BDBCB> destination );
		bool isClosing() const;
		void isClosing( bool target );

		void printFilter();
		void printHeaderValidationPendingQueue();
		void printMergePendingQueue();

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
  BDVirtualChain( std::shared_ptr<LightUTXOSet> utxoSet , 
									std::shared_ptr<BlockLocalStrageManager> localStrageManager ,
									std::shared_ptr<block::Block> startBlock ,
									std::shared_ptr<unsigned char> stopHash = nullptr );

	BDVirtualChain(
									std::shared_ptr<LightUTXOSet> utxoSet,
									std::shared_ptr<BlockLocalStrageManager> localStrageManager,
									std::shared_ptr<block::Block> startBlock,
									std::vector< std::shared_ptr<block::BlockHeader>> _headerVector // チェーンの順に並んでいること
									
									);

	~BDVirtualChain();

  void requestedExtendChain( std::function<struct BDBCB( std::shared_ptr<unsigned char> )> findPopCallback ); // 仮想チェーンの最先端ブロックハッシュを元にポップする

	bool startSequentialAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC ); // 主にIBD　ローカルファイルに書き込みながらチェーンを組み立てる
	bool startParallelAssemble( std::shared_ptr<StreamBufferContainer> toRequesterSBC ); // 主にマージ ローカルファイルには保存せずブロックをすべてダウンロードする
	

	bool add( std::shared_ptr<block::BlockHeader> header );
	bool add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector );
	bool add( std::shared_ptr<block::Block> block );

	size_t chainLength();
	void printVirtualChain();
	void printFilter();
	void printHeaderValidationPendingQueue();
	void printMergePendingQueue();
};




};



#endif
