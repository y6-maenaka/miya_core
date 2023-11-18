#include "./virtual_chain.h"

#include "../block/block.h"
#include "../block/block_header.h"


namespace miya_chain
{







/*
  ------------------------------------------------------------------------------
    - (Layer1) : Filter
  ------------------------------------------------------------------------------
*/



bool BDFilter::add( struct BDBCB targetBCB ) // BDBCBは変更しない
{
	if( targetBCB.block == nullptr ) return false;
	struct BlockHashAsKey key{ targetBCB }; // BDBCBからキーの作成

  auto ret =  _filter._filterMap.insert( {key , std::make_pair(targetBCB, nullptr)} );
  if( ret.second )  // 要素が存在せず,新たに追加した場合
  {
		std::cout << "(BDFilter) 新規追加" << "\n";
		_validationHeaderQueue.push( std::make_pair( key , targetBCB ) ); // ヘッダ検証コンポーネントに移動する
    	_validationHeaderQueue._cv.notify_all(); // leyer2を監視するスレッドが寝ている可能性があるので
  }else
	{
		std::cout << "(BDFilter) 重複要素" << "\n";
		return false;
	}

  return true;
}



// VirtualChainに要素を追加するインターフェースは基本的にここだけにする
// 複数個一気にAddしてくれるとかなり処理効率が上がる // 引数ごとvectorにするか
bool BDFilter::add( std::shared_ptr<block::BlockHeader> header ) // これが1番のフィルター機能になる
{
	struct BDBCB targetBCB;
	targetBCB.header( header );
	targetBCB.status = static_cast<int>(BDState::BlockHeaderReceived);

	return this->add( targetBCB );
}



bool BDFilter::add( std::vector< std::shared_ptr<block::BlockHeader> > headerVector )
{
	bool insertFlag = false;
	for( auto itr : headerVector )
	{
		std::shared_ptr<unsigned char> blockHash;
		struct BDBCB cb; cb.header( itr );
		cb.status = static_cast<int>(BDState::BlockHeaderReceived);
		struct BlockHashAsKey key{ cb };

		auto ret = _filter._filterMap.insert( {key, std::make_pair(cb, nullptr )} );
		if( ret.second ){
			_validationHeaderQueue.push( std::make_pair( key , cb ) );
			insertFlag = true; // 少なくとも一つ追加したらnotify_allを呼び出す必要があるため
		}
	}

	if( insertFlag )
		_validationHeaderQueue._cv.notify_all(); // すべて追加した後に通知する
}



bool BDFilter::add( std::shared_ptr<block::Block> block ) // フィルタにブロックへの直ポインタがある場合はそっちを更新する
{
	std::shared_ptr<unsigned char> blockHash;
	block->blockHash( &blockHash );
	struct BlockHashAsKey key( blockHash );

	auto ret = _filter._filterMap.find( key );
	if( ret == _filter._filterMap.end() ) return false; // 対象が存在しない場合は破棄
	if( ret->second.first.status != static_cast<int>(BDState::BlockHeaderValidated) ) return false; // ブロックヘッダの検証が済んでいない,もしくはブロック本体が既に到着している場合は破棄

	// 直接チェーンに繋がれているBCBDBを操作する
	ret->second.first.block = block; // 到着したブロックで上書きする
	ret->second.first.status = static_cast<int>(BDState::BlockBodyReceived); // ステータスを変更する

	return true;
}


bool BDFilter::isClosing() const
{
  return _filter._closeing;
}












/*
  ------------------------------------------------------------------------------
    - (Layer2) : ValidationHeaderQueue
  ------------------------------------------------------------------------------
*/


void BDFilter::ValidationHeaderQueue::push( std::pair< BlockHashAsKey, struct BDBCB > targetPair )
{
	std::unique_lock<std::mutex> lock(_mtx);
	_pendingQueue.push_back( targetPair );
}













/*
  ------------------------------------------------------------------------------
    - (Layer3) : PendingMergeQueue
  ------------------------------------------------------------------------------
*/



// ブロックのprevOutを比較・一致対象とし検索取り出しする
struct BDBCB BDFilter::PendingMergeQueue::findPop( std::shared_ptr<unsigned char> prevBlockHash )
{
	std::unique_lock<std::mutex> lock(_mtx);
	struct BDBCB ret;
  	PrevBlockHashAsKey key( prevBlockHash );

  	auto retItr = _pendingMap.find( key );
  	if( retItr == _pendingMap.end() ||  // 要素が存在しないか検証が済んでいない場合は空を返却する
		retItr->second.status <= static_cast<int>(BDState::BlockHeaderReceived)  // このキューに入っている時点で検証が済んでいないことはない
		)
  	{
    	ret.status = static_cast<int>( BDState::Empty ); // 空(要素なし)を設定してリターン
		return ret;
  	}
	
	ret = retItr->second;
  	_pendingMap.erase( retItr ); // ポップする
	return ret;
}


void BDFilter::PendingMergeQueue::push( struct BDBCB cb )
{
	std::unique_lock<std::mutex> lock(_mtx);
  	PrevBlockHashAsKey key( cb );

	cb.status = static_cast<int>(BDState::BlockHeaderValidated);
  	_pendingMap.insert( {key, cb} ); // 検証が済み,仮想チェーンに取り込み待ちの要素を挿入する
}







/*
  ------------------------------------------------------------------------------
    - BDFilter
  ------------------------------------------------------------------------------
*/



BDFilter::BDFilter( BDVirtualChain* virtualChain )
{
	_virtualChain = virtualChain;
	assert(_virtualChain != nullptr );

	// 受信ヘッダ検証用スレッド
	std::thread headerValidator([&]() // 起動と同時に検証用スレッドを起動する
  {
	for(;;)
	{
		std::cout << "BDFilter::Constructor headerValidator started" << "\n";
		std::unique_lock<std::mutex> lock(_validationHeaderQueue._mtx);
		_validationHeaderQueue._cv.wait( lock , [&]{
				return !(_validationHeaderQueue._pendingQueue.empty()); // 空でなければ解除
		});
		std::cout << "(BDFilter) Layer1検証プロセス起動" << "\n";

	for( auto itr = _validationHeaderQueue._pendingQueue.begin() ; itr != _validationHeaderQueue._pendingQueue.end() ;)
	{
		std::pair< BlockHashAsKey , struct BDBCB > frontCBSet;
		frontCBSet = (_validationHeaderQueue._pendingQueue.front()); // キュー先頭から検証する
		itr++; // eraseする前に先に進めとく
		_validationHeaderQueue._pendingQueue.erase( _validationHeaderQueue._pendingQueue.begin() ); // ここでeraseしたらauto itrバグらない？

		// 既に保存済みの場合は検証もlayer2への追加もしない( ただフィルターに追加するだけに留める )
		if( frontCBSet.second.status == static_cast<int>(BDState::BlockStored) ) continue;

		if( !(frontCBSet.second.block->header()->verify()) ){ // ブロックヘッダのみを簡易的に検証する
			std::cout << "(BDFilter) ブロックヘッダ検証失敗" << "\n";
			continue; // 検証失敗したら破棄
		}

		std::cout << "(BDFilter) ブロックヘッダ検証成功" << "\n";
		_pendingMergeQueue.push( frontCBSet.second  ); // ブロックダウンロードとブロック検証を担当するlayer2に移動する
	}
		// layer2要素が空でなければvirtualChainに通知する
		if( _pendingMergeQueue._pendingMap.size() <= 0 ) continue;

		std::function<struct BDBCB( std::shared_ptr<unsigned char> )> popCallback = std::bind( &PendingMergeQueue::findPop ,
																						std::ref(_pendingMergeQueue),
																						std::placeholders::_1
		);
		// 検証が完了したら、virtualChainのcallbackを呼び出してチェーンに取り込んでもらう
		_virtualChain->requestedExtendChain( popCallback );
		// std::bind() 引数の固定
	}

  });

  headerValidator.detach();
  std::cout << "BDFilter::Constructor headerValidator detached" << "\n";
}








};
