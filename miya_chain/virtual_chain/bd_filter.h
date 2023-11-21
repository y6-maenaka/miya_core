#ifndef B18AE406_2528_4F4B_BEC6_FEF8FEDB4408
#define B18AE406_2528_4F4B_BEC6_FEF8FEDB4408


#include "./virtual_chain.h"



namespace miya_chain
{

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
 			bool _headerDownloadClosing = false; // 現時点でフィルターに登録していない要素は追加しない（破棄する）
			bool _blockDownloadClosing = false;

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
			bool empty();
	} _pendingMergeQueue;


	BDVirtualChain* _virtualChain;

public:
    BDFilter( BDVirtualChain* virtualChain ); // ヘッダを検証するスレッドを起動する

		/* いずれもlayer1に対する操作 ユーザプログラムから直接layer2を操作することはない */
		bool add( struct BDBCB targetBCB );
        bool add( std::vector<struct BDBCB> cbVector );
        /*
        bool add( std::vector<struct BDBCB> cbVector );
        bool add( std::vector<std::shared_ptr<struct BDBCB>> targetCBVector );
		bool add( std::shared_ptr<block::BlockHeader> header);
		bool add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector );
		bool add( std::shared_ptr<block::Block> block );
        */

		// 仮想チェーンにブロックヘッダが取り込まれ,ブロック本体ダウンロードする際は,フィルタの中間層を飛ばして直接仮想チェーンで管理されているBDBに変更を加える
		void updateBlockPtr( std::shared_ptr<struct BDBCB> destination );
		bool isHeaderDownloadClosing() const;
		void isHeaderDownloadClosing( bool target );
		bool isBlockDownloadClosing();
		void isBlockDownloadClosing( bool target );

		void printFilter();
		void printHeaderValidationPendingQueue();
		void printMergePendingQueue();

};




};

#endif // B18AE406_2528_4F4B_BEC6_FEF8FEDB4408
