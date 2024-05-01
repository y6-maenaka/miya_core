#ifndef CF253308_7C86_479C_8BEE_58DCE17E283E
#define CF253308_7C86_479C_8BEE_58DCE17E283E


#include <memory>
#include <iostream>


#include <stream_buffer/stream_buffer.h>
#include <stream_buffer/stream_buffer_container.h>

namespace chain
{

class HeaderStore;
class TransactionStore;


class BlockChainManager
{

private:
	struct
	{
		std::shared_ptr<HeaderStore> _headerStore;
		std::shared_ptr<TransactionStore> _txStore;
	} _store ;


	std::shared_ptr<StreamBufferContainer> _toSenderSB; // ノードへのsenderパイプ
	std::shared_ptr<StreamBufferContainer> _incomingSB; // 本ノードへのリクエストが入ってくる


public:
	BlockChainManager( std::shared_ptr<HeaderStore> headerStore , std::shared_ptr<TransactionStore> txStore );

	bool syncBlockHeader();
	bool syncBlock(); // 最先端のブロックまで自身のチェーンを同期させる

	bool InitialiBlockDownload(); // IBD // ノードのネットワーク参加時に最新チェーンを取得するために行われる
};


};

/*
	- ブロックの取り込み
	- ブロック先端が伸びたことによる新たなブロックのマイニング開始( transactionPoolより )
	- ブロックvalidatorから通知が来たらチェーンに繋げて適切なutxoを作成する
	- 検証済みブロックが自信が採掘しているチェーンより先を行っていたらそのチェーンを優先する
	- wrongChainに取り込んでしまっているブロックに含まれているトランザクションをmempoolに戻す
	- utxoも戻す
*/



/* ブロックチェーンの順序はどのように保存するか */
// >>>> chain head
// 最後に保存したchain headが不正チェーンヘッドだった場合は他ノードに問い合わせてもレスポンスがない



// 一番初めにブロックヘッダで同期を目指す
// ブロックインベントリ
// ジェネシスブロックはハードコードされている





/* [  IBDの簡易的なプロセス ]

 1. 接続しているノードから最先端のブロックヘッダを取得する
 ※　ブロックヘッダはキャッシュデータベースに保存しておく
 2. 自身のブロックヘッダデータベースにIDで検索する
 (ない場合)    | (ある場合)
	prevで検索する | 検索を終了してデータベースキャッシュから本データベースへ写す
 */









#endif // CF253308_7C86_479C_8BEE_58DCE17E283E
