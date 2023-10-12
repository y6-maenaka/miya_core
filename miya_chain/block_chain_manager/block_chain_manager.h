#ifndef CF253308_7C86_479C_8BEE_58DCE17E283E
#define CF253308_7C86_479C_8BEE_58DCE17E283E





class BlockChainManager{

};


/*
	- ブロックの取り込み
	- ブロック先端が伸びたことによる新たなブロックのマイニング開始( transactionPoolより )
	- ブロックvalidatorから通知が来たらチェーンに繋げて適切なutxoを作成する
	- 検証済みブロックが自信が採掘しているチェーンより先を行っていたらそのチェーンを優先する
	- wrongChainに取り込んでしまっているブロックに含まれているトランザクションをmempoolに戻す
	- utxoも戻す 
*/





#endif // CF253308_7C86_479C_8BEE_58DCE17E283E



