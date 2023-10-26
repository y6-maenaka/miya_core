rev.dat ファイルはblkxxxx.datとセットで生成されるundoデータを保存するファイル

UTXO : トランザクションの出力がまだ使用可能かをトラッキングするために使用される
UNDO : トランザクションの逆操作を可能にし,過去のトランザクションがブロックチェーンから削除された場合に,utxoセットを正確に復元するために役立つ
			 


※ undoデータについて
トランザクションがブロックに含まれると,そのトランザクションは永久的に記録され,ブロックチェーン上で不可逆的となる.　しかし,トランザクションが含まれる前に,そのトランザクションの影響を管理するために"undo data"が生成される

※ undo dataにはトランザクションが実行される前のutxoセットの状態が含まれている. 

※ ブロックチェーンからブロックが削除される場合,それに含まれる全てのトランザクションの影響を元に戻す必要がある. 

※ ブロックに取り込まれて削除される前のutxoのスナップショット



つまり,block作成とblock保存の流れ

1. ブロックヘッダの取り寄せ
1.1 検証
2. ブロックデータの取り寄せ
	2.1 検証(utxoへの問い合わせ)
	2.2 mempool(メモリ)へストア
3. undoファイルの作成
4. blocks/ にストア
5. utxoからの削除



