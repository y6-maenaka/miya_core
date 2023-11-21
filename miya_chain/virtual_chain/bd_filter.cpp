#include "bd_filter.h"
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


/*
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
*/


bool BDFilter::add( std::vector<struct BDBCB> cbVector  )
{
	std::cout << "BDFilter::add() called" << "\n";
	bool insertFlag = false;
	for( auto itr : cbVector )	
	{
		struct BlockHashAsKey key(itr);
		switch( itr.status )
		{

			case static_cast<int>(BDState::BlockHeaderReceived): // ヘッダー受信の際
			{
				auto ret =  _filter._filterMap.insert( {key , std::make_pair( itr, nullptr)} );
				if( ret.second ) // 要素が存在していない場合
				{
					std::cout << "(BDFilter) 新規追加" << "\n";
					_validationHeaderQueue.push( std::make_pair( key , itr ) ); // ヘッダ検証コンポーネントに移動する
					insertFlag = true;
				}
				else{
					continue;
				}
				break;
			}

			case static_cast<int>(BDState::BlockBodyReceived): // ブロック受信の際
			{
				auto ret = _filter._filterMap.find( key );
				if( ret == _filter._filterMap.end() )
				{
					std::cout << "直ポインタが存在しません" << "\n";
					continue;
				}			 
				if( ret->second.second->status != static_cast<int>(BDState::BlockHeaderValidated) )
				{
					std::cout << ret->second.first.status << "\n";
					std::cout << "ブロックヘッダの検証が済んでいません" << "\n";
					continue;
				} 

				std::cout << "直ポインタを上書きします" << "\n";
				ret->second.second->block = itr.block;
				ret->second.second->status = static_cast<int>(BDState::BlockBodyReceived); // ステータスを変更する
				std::cout << "直ポインタを上書きしました" << "\n";
				break	;
			}

			case static_cast<int>(BDState::BlockStored): // フィルターにキャッシュすすまでで留める
			{
				auto ret = _filter._filterMap.find( key );	
				if( ret == _filter._filterMap.end() ){
					std::cout << "対象の要素が存在しません" << "\n";
					if( !(isHeaderDownloadClosing()) && !(isBlockDownloadClosing()) )
					{
						_filter._filterMap.insert( {key, std::make_pair(itr,nullptr) } );
						insertFlag = true;
					}
					else
					{
						continue;
					}

				}
				break;
			}

		}
	}


	if( insertFlag ){
		_validationHeaderQueue._cv.notify_all(); // すべて追加した後に通知する))
	}
		return true;
}






void BDFilter::updateBlockPtr( std::shared_ptr<struct BDBCB> destination )
{
	struct BlockHashAsKey key( *destination );
	auto mached = _filter._filterMap.find( key );
	if( mached == _filter._filterMap.end() ) return;
	mached->second.second = destination;
}


bool BDFilter::isHeaderDownloadClosing() const
{
  return _filter._headerDownloadClosing;
}

void BDFilter::isHeaderDownloadClosing( bool target )
{
	_filter._headerDownloadClosing = target;
}

bool BDFilter::isBlockDownloadClosing()
{
	return _filter._blockDownloadClosing;
}

void BDFilter::isBlockDownloadClosing( bool target )
{
	_filter._blockDownloadClosing = target;
}




void BDFilter::printFilter()
{
	int i = 0;
	for( auto itr : _filter._filterMap )
	{
		std::cout << "\n";
		std::cout << "(" << i << ") " << "\n";
		std::cout << " [key] : ";
		for( int i=0; i<32; i ++ ) 
			printf("%02X", itr.first._blockHash[i] );
		std::cout << "\n";

		std::cout <<  " [block hash] :: ";
		for( int i=0; i<32; i++ )
			printf("%02X", itr.second.first.blockHash().get()[i]);
		std::cout << "\n";

		if( itr.second.second == nullptr ) std::cout << " [cache] :: None" << "\n";
		else std::cout << "[cache] :: active" << "\n";
		std::cout << "\n";
		i++;
	}
}


void BDFilter::printHeaderValidationPendingQueue()
{
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
	std::cout << "[ Header Validation Pending Queue ]" << "\n";
	std::cout << "Pending Count :: " << _validationHeaderQueue._pendingQueue.size() << "\n";
	int i=0;
	for( auto itr : _validationHeaderQueue._pendingQueue )
	{
		std::cout << "\n";
		std::cout << "(" << i << ") " << "\n";
		std::cout << " [key] : ";
		for( int i=0; i<32; i ++ ) 
			printf("%02X", itr.first._blockHash[i] );
		std::cout << "\n";

		printf("%p\n", itr.second.block.get() );
		printf("%p\n", itr.second.blockHash().get() );

		std::cout <<  " [block hash] :: ";
		for( int i=0; i<32; i++ )
			printf("%02X", itr.second.blockHash().get()[i]);
		std::cout << "\n";

		i++	;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
}


void BDFilter::printMergePendingQueue()
{
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
	std::cout << "[ Merge Pending Queue ]" << "\n";
	std::cout << "Pending Count :: " << _pendingMergeQueue._pendingMap.size() << "\n";
	int i=0;
	for( auto itr : _pendingMergeQueue._pendingMap )
	{
		std::cout << "\n";
		std::cout << "(" << i << ") " << "\n";
		std::cout << " [key] : ";
		for( int i=0; i<32; i ++ ) 
			printf("%02X", itr.first._prevBlockHash[i] );
		std::cout << "\n";

		std::cout <<  " [block hash] :: ";
		for( int i=0; i<32; i++ )
			printf("%02X", itr.second.blockHash().get()[i]);
		std::cout << "\n";

		i++	;
	}
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
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


bool BDFilter::PendingMergeQueue::empty()
{
	std::unique_lock<std::mutex> lock(_mtx);
	return _pendingMap.empty();
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
				bool ret =  !(_validationHeaderQueue._pendingQueue.empty());
				std::cout << "検証完了" << "\n";
				return ret;
		});
		std::cout << "(BDFilter) Layer1検証プロセス起動" << "\n";

	for( auto itr = _validationHeaderQueue._pendingQueue.begin() ; itr != _validationHeaderQueue._pendingQueue.end() ;)
	{
		std::cout << "バックグラウンド検証プロセス起動" << "\n";
		std::pair< BlockHashAsKey , struct BDBCB > frontCBSet;
		frontCBSet = (_validationHeaderQueue._pendingQueue.front()); // キュー先頭から検証する
		// itr++; // eraseする前に先に進めとく
		itr = _validationHeaderQueue._pendingQueue.erase( itr ); // ここでeraseしたらauto itrバグらない？
		
		// 既に保存済みの場合は検証もlayer2への追加もしない( ただフィルターに追加するだけに留める )
		if( frontCBSet.second.status == static_cast<int>(BDState::BlockStored) ){
			std::cout << "フィルタに追加します" << "\n";
			continue;
		} 

		if( !(frontCBSet.second.block->header()->verify()) ){ // ブロックヘッダのみを簡易的に検証する
			std::cout << "(BDFilter) ブロックヘッダ検証失敗" << "\n";
			continue; // 検証失敗したら破棄
		}

		std::cout << "(BDFilter) ブロックヘッダ検証成功" << "\n";
		_pendingMergeQueue.push( frontCBSet.second  ); // ブロックダウンロードとブロック検証を担当するlayer2に移動する
		std::cout << "pendingQueue count :: "	 << _validationHeaderQueue._pendingQueue.size() << "\n";
	}
		// layer2要素が空でなければvirtualChainに通知する
		if( _pendingMergeQueue.empty() ) continue;

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
