#ifndef B18AE406_2528_4F4B_BEC6_FEF8FEDB4408
#define B18AE406_2528_4F4B_BEC6_FEF8FEDB4408

#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <functional>

#include "./BDBCB.h"
#include "./block_hash_as_key.h"
#include "./prev_block_hash_as_key.h"


namespace block
{
	struct Block;
	struct BlockHeader;
};




namespace miya_chain
{
class BDVirtualChain;


using FilterStateUpdator = std::function< void(std::shared_ptr<unsigned char>, int) >;




class BDFilter
{
private:
	std::mutex _mtx;
	std::condition_variable _cv;
	bool _isClose = true;

	std::unordered_map< BlockHashAsKey, std::shared_ptr<struct BDBCB> , BlockHashAsKey::Hash > _filter;
public:
	BDFilter();

	std::shared_ptr<struct BDBCB> filter( std::shared_ptr<unsigned char> target, bool isReject = false );
	std::shared_ptr<struct BDBCB> filter( std::shared_ptr<block::Block> target, bool isReject = false );
	std::shared_ptr<struct BDBCB> filter( std::shared_ptr<block::BlockHeader> target, bool isReject = false );

	void mask( std::vector< std::shared_ptr<block::BlockHeader>> targetVector ); // かなり重い処理
	void update( std::shared_ptr<unsigned char> blockHash , int state ); //filterに登録しているBDBCBの内部状態を更新する

	void __printFilter();
};






/*
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
    // ヘッダの検証スレッドも宣言と同時に起動される

	class ValidationHeaderQueue // 受信したブロックヘッダが待機させられるバッファ
	{
		public:
			std::vector< std::pair<BlockHashAsKey , struct BDBCB > >  _pendingVector;

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

		// いずれもlayer1に対する操作 ユーザプログラムから直接layer2を操作することはない
		// bool add( struct BDBCB targetBCB );
    bool add( std::vector<struct BDBCB> cbVector );

		// 仮想チェーンにブロックヘッダが取り込まれ,ブロック本体ダウンロードする際は,フィルタの中間層を飛ばして直接仮想チェーンで管理されているBDBに変更を加える
		void updateBlockPtr( std::shared_ptr<struct BDBCB> destination );

		bool isHeaderDownloadClosing() const;
		void isHeaderDownloadClosing( bool target );
		bool isBlockDownloadClosing() const;
		void isBlockDownloadClosing( bool target );

		// debug
		void printFilter();
		void printHeaderValidationPendingQueue();
		void printMergePendingQueue();
};
*/



};

#endif // B18AE406_2528_4F4B_BEC6_FEF8FEDB4408
