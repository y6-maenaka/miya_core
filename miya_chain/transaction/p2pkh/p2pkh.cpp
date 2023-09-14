#include "p2pkh.h"

#include "../tx/tx_in.h"
#include "../tx/tx_out.h"
#include "../script/signature_script.h"
#include "../script/pk_script.h"
#include "../../../shared_components/cipher/ecdsa_manager.h"
#include "../../../shared_components/hash/sha_hash.h"


#include "openssl/bio.h" 
#include "openssl/evp.h"


namespace tx{






unsigned short P2PKH::inCount()
{
	return _body._ins.size();
}



unsigned short P2PKH::outCount()
{
	return _body._outs.size();
}





std::vector< std::shared_ptr<TxIn> > P2PKH::ins()
{
	return _body._ins;
}




std::vector< std::shared_ptr<TxOut> > P2PKH::outs()
{
	return _body._outs;
}




bool P2PKH::sign()
{
	// このトランザクションが含んでいる全てのtx_inに対して署名を行う
	//
	// 多分coinbaseInputは全てそのまま(値をセットしている状態)で書き出して大丈夫

	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxInVector;
	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxOutVector;
	std::shared_ptr<unsigned char> exportexRawTx; unsigned int exportedRawTxLength;
	std::shared_ptr<unsigned char> rawSign; unsigned int rawSignLength = 0;
	std::shared_ptr<unsigned char> sign; unsigned int signLength = 0;
	unsigned int formatPtr = 0;


	for( int i=0; i<_body._ins.size(); i++ )
	{

		for( int j=0; j<_body._ins.size(); j++ )
		{
			if( i == j ) // 公開鍵ハッシュを格納する
				rawSignLength = _body._ins.at(j)->exportRawWithSignatureScript( exportexRawTx );
			else
				rawSignLength = _body._ins.at(j)->exportRawWithEmpty( exportexRawTx );

			exportedRawTxInVector.push_back( std::make_pair(exportexRawTx,exportedRawTxLength) );
		}
		
		for( auto itr : _body._outs )
		{
			exportedRawTxLength = itr->exportRaw( exportexRawTx );
			exportedRawTxOutVector.push_back( std::make_pair(exportexRawTx, exportedRawTxLength ) );
		}



		formatPtr = 0;
		unsigned int exportedRawTxInsLength = 0; std::shared_ptr<unsigned char> exportedRawTxIns; 
		for( auto itr : exportedRawTxInVector )
		{
			memcpy( exportedRawTxIns.get() + formatPtr , itr.first.get(), itr.second ); formatPtr += itr.second;
			exportedRawTxInsLength += itr.second;
		} 


		formatPtr = 0;
		unsigned int exportedRawTxOutsLength = 0; std::shared_ptr<unsigned char> exportedRawTxOuts;
		for( auto itr : exportedRawTxOutVector ) 
		{
			memcpy( exportedRawTxOuts.get() + formatPtr , itr.first.get(), itr.second ); formatPtr += itr.second;
			exportedRawTxOutsLength += itr.second;
		} 


		formatPtr = 0;
		unsigned int exportedRawLength; std::shared_ptr<unsigned char> exportedRaw;
		exportedRawLength =  sizeof(_body._version) + exportedRawTxInsLength + exportedRawTxOutsLength;
		memcpy( exportedRaw.get() , &(_body._version) , sizeof(_body._version) );
		memcpy( exportedRaw.get() + formatPtr , exportedRawTxIns.get() , exportedRawTxInsLength ); formatPtr += exportedRawTxInsLength;
		memcpy( exportedRaw.get() + formatPtr , exportedRawTxOuts.get(), exportedRawTxOutsLength ); formatPtr += exportedRawTxOutsLength;

		// 書き出したデータをハッシュして,秘密鍵でエンクリプトする
		std::shared_ptr<unsigned char> hashedExportedRaw; unsigned int hashedExportedRawLength = 0;
		std::shared_ptr<unsigned char> encryptedExportedRaw; unsigned int encryptedExportedRawLength;
		hashedExportedRawLength = hash::SHAHash( exportedRaw , exportedRawLength , hashedExportedRaw , "sha1" );

		// 署名値を生成する
		signLength = cipher::ECDSAManager::sign( exportedRaw, exportedRawLength, _body._ins.at(i)->pkey(), sign );
		_body._ins.at(i)->sign( sign, signLength , true );
	}


}







unsigned int P2PKH::exportRaw( std::shared_ptr<unsigned char> retRaw )
{
	// 全てのtx_inの署名が住んでいることを確認する

}



/*
int P2PKH::TxInCnt()
{
	return _body._ins.size(); }


int P2PKH::TxOutCnt() // getter
{
	return _body._outs.size();
}



// raw to structure
P2PKH::P2PKH( unsigned char* rawP2PKHBuff , unsigned int rawP2PKHBuffSize )
{

	unsigned int pos = 0;

	memcpy( &(_body._version) , rawP2PKHBuff , sizeof(_body._version) );  pos += sizeof(_body._version);  // versionの取り込み

	uint16_t InOutCnt;
	
	// = Ins の取り込み ================================================================= 
	// ins count 
	memcpy( &InOutCnt  ,rawP2PKHBuff  + pos , sizeof(InOutCnt) );  pos += sizeof(InOutCnt); 

	std::cout << "-> " << ntohs( InOutCnt ) << "\n";
	std::cout << "--> " << ntohs( InOutCnt ) << "\n";

	unsigned char *rawPrevOutBuff = nullptr;
	TxIn *newTxIn;

	for( int i=0; i < ntohs(InOutCnt) ; i++ )
	{							
		newTxIn = new TxIn;

		// なぜかこれを追加すると　TxIn->signatureScript が書き換えられる　  // ヒープ領域のメモリ不足で勝手に解放しちゃってる？
		 // memcpy( rawPrevOutBuff, rawP2PKHBuff + pos  , PREV_OUT_SIZE );  pos += PREV_OUT_SIZE;
		//newTxIn->prevOut( rawPrevOutBuff, PREV_OUT_SIZE );
		//delete rawPrevOutBuff; rawPrevOutBuff = nullptr;
	
		pos += newTxIn->autoTakeInSignatureScript( rawP2PKHBuff + pos ); // 戻り値が正常に取り込んだsignatureScriptのサイズになる
		_body._ins.push_back( newTxIn );
	} 
	// insの取り込み完了 ==========================================================================

	std::cout << "Take In TxIn from RawP2PKH DONE" << "\n";


	// = Outs の取り込み ================================================================= 
	// outs count 
	memcpy( &InOutCnt  , rawP2PKHBuff + pos , sizeof(InOutCnt) );  pos += sizeof(InOutCnt);  //　4bytesの差

	for( int i=0; i < ntohs(InOutCnt) ; i++ ){
		TxOut *newTxOut = new TxOut;
		uint64_t value; // あまりスマートではないやり方
										
		memcpy( &(newTxOut->_value) , rawP2PKHBuff + pos , sizeof( TxOut::_value) );  pos += sizeof( TxOut::_value );
	
		pos += newTxOut->autoTakeInPkScript( rawP2PKHBuff + pos ); 
		_body._outs.push_back( newTxOut );

		// 本来はscript_bytesまでもscriptで持たせてtakeInScriptメソッドで済ませた方がまとまりが良い
	}
	// outsの取り込み完了 ==========================================================================

	std::cout << "Take In TxOut from RawP2PKH DONE" << "\n";

	return;
}


//cher up Yuri San, 加油小弟、がんばれカッパくん。 2023.4.9 -kushima


void P2PKH::addInput( TxIn *txIn )
{
	_body._ins.push_back( txIn );
}


void P2PKH::addOutput( TxOut* txOut )
{
	_body._outs.push_back( txOut );
}





bool P2PKH::sign()
{

	//unsigned char* sig = NULL;
	//unsigned int sigSize = 0;

	unsigned char* partialBuff = NULL;
	unsigned int partialBuffSize = 0;

	unsigned char* sigBuff; unsigned int sigBuffSize = 0;

	//int i=0;
 	//for (auto& itr : _body._ins ) 
	for( int i=0; i< _body._ins.size(); i++)	
	{
		partialBuffSize = exportPartialRaw( &partialBuff , i ); // 署名用に該当するTxにDestPubKeyをそれ以外は空白にしたTxInにしてトランザクション全体を書き出す		

		sigBuffSize =  cipher::ECDSAManager::Sign( partialBuff , partialBuffSize , &sigBuff , _body._ins.at(i)->pkey() );

		_body._ins.at(i)->sig( sigBuff );
		_body._ins.at(i)->sigSize(  sigBuffSize ) ;

		//i++;
 	}


	
	isSigned = true;
	return true;
}



bool P2PKH::verify()
{
	unsigned char* partialBuff = nullptr; unsigned int partialBuffSize = 0;
	bool flag = false;

	int i=0;
	for( auto& itr : _body._ins ){

		partialBuffSize = exportPartialRaw( &partialBuff , i );

		unsigned char
		flag = cipher::ECDSAManager::Verify( itr->sig() , itr->sigSize(), partialBuff, partialBuffSize , itr->pkey() );

		if( flag == false ) return false;
		i++;
	}

	return true;
}





// 一箇所だけ公開鍵をセットしたRawTxInを返す
unsigned int P2PKH::exportPartialTxIn( unsigned char **ret , int index )
{
	unsigned char *exportTxInBuff = NULL;
	unsigned int exportTxInBuffSize = 0;


	*ret = new unsigned char[ exportPartialTxInSize(index) ];

	unsigned int pos = 0;


	for( int i=0 ; i < _body._ins.size() ; i++ ){

		// なぜ最初からvector(_script)に要素が入っているのか？ -> create関数の中でclearすることで一時的に対応	

		if( i == index ) // 目的のInxに到達した公開鍵ハッシュをセットする
		{
			_body._ins.at(i)->signatureScript( _body._ins.at(i)->signatureScript()->createHashedPubKeyScript( _body._ins.at(i)->pkey()) );
			exportTxInBuffSize = _body._ins.at(i)->exportRaw( &exportTxInBuff );
		}
		else{
			exportTxInBuffSize = _body._ins.at(i)->exportEmptyRaw( &exportTxInBuff ); // signature script empty tx in
		}

		memcpy( *ret + pos , exportTxInBuff , exportTxInBuffSize ); pos += exportTxInBuffSize;

		delete exportTxInBuff;   exportTxInBuff = nullptr;
	}

	return exportPartialTxInSize( index );
};




unsigned int P2PKH::exportPartialTxInSize( int index ){

	unsigned int exportTxInSize = 0;

	for( int i=0; i<_body._ins.size(); i++ )
	{
		if( i == index ){
			_body._ins.at(i)->signatureScript( _body._ins.at(i)->signatureScript()->createHashedPubKeyScript( _body._ins.at(i)->pkey() ) );
			exportTxInSize += _body._ins.at(i)->exportRawSize();
		}
		else{
			exportTxInSize += _body._ins.at(i)->exportEmptyRawSize();
		}
	}
	return exportTxInSize;
}






unsigned int P2PKH::exportPartialRaw( unsigned char **ret , int partialTxInIndex )
{	
	*ret = new unsigned char[ exportPartialRawSize( partialTxInIndex ) ];

	unsigned int pos = 0;
	uint16_t InOutCnt;

	memcpy( *ret  , &(_body._version) , sizeof(_body._version) ); pos += sizeof(_body._version); // versionの書き出し


	// = TxIn関連の書き出し =======================================================

	// カウントの書き出し
 	InOutCnt = htons( _body._ins.size() );
	memcpy(  *ret + pos , &InOutCnt ,  sizeof(InOutCnt) ); pos += sizeof(InOutCnt);
	// TxIns の書き出し 
	unsigned char* exportPartialTxInRawBuff; unsigned int exportPartialTxInRawBuffSize = 0;
	exportPartialTxInRawBuffSize = exportPartialTxIn( &exportPartialTxInRawBuff , partialTxInIndex );
	memcpy( *ret + pos , exportPartialTxInRawBuff , exportPartialTxInRawBuffSize );  pos += exportPartialTxInRawBuffSize;




	// = TxOut関連の書き出し =======================================================

	// カウントの書き出し
	InOutCnt = htons( _body._outs.size() );
	memcpy( *ret + pos , &InOutCnt , sizeof(InOutCnt) ); pos += sizeof(InOutCnt);
	// TxOuts の書き出し 
	unsigned char* exportTxOutRawBuff = NULL; unsigned int exportTxOutRawBuffSize = 0;
	exportTxOutRawBuffSize = exportTxOutRaw( &exportTxOutRawBuff ); 
	memcpy( *ret + pos , exportTxOutRawBuff , exportTxOutRawBuffSize ); pos += exportTxOutRawBuffSize;

	return exportPartialRawSize( partialTxInIndex );
}





unsigned int P2PKH::exportPartialRawSize( int partialTxInIndex )
{
				 // len( version )  +  len( in_count )   + len( out_count ) + len( partialTxIn ) + len( rawTxOut )
	return sizeof(_body._version) + sizeof(uint16_t) + sizeof(uint16_t) + exportPartialTxInSize( partialTxInIndex ) + exportTxOutRawSize();
}






unsigned int P2PKH::exportTxInRaw( unsigned char **ret )
{		
	*ret = new unsigned char[ exportTxInRawSize() ];

	unsigned char* exportTxInBuff = NULL; unsigned int exportTxInBuffSize = 0;
	unsigned int pos = 0;

	//unsigned char* sigBuff = nullptr; unsigned int sigBuffSize = 0;


	int i=0;
	for( auto &itr : _body._ins ){

		// 署名値が格納されているTxInだった場合はそのまま書き出しサイズを計算する
		if( itr->signatureScript()->scriptType() == static_cast<int>(SCRIPT_TYPE::Sig) )
		{
			exportTxInBuffSize = itr->exportRaw( &exportTxInBuff );
		}
		else{
			// 最終的にTxInに署名が格納されていないからここで格納する
			itr->signatureScript( itr->signatureScript()->createSigScript( itr->sig() , itr->sigSize(), itr->pkey() )  ); 
			exportTxInBuffSize = itr->exportRaw( &exportTxInBuff );
		}
	
		memcpy( *ret + pos , exportTxInBuff, exportTxInBuffSize );  pos += exportTxInBuffSize;
		delete exportTxInBuff; exportTxInBuff = nullptr;
		i++;
	}


	return exportTxInRawSize();
}




unsigned int P2PKH::exportTxInRawSize()
{
	if ( isSigned == false ) return 0;
	unsigned int exportRawSize = 0;

	for( auto &itr : _body._ins ){

		// 署名値が格納されているTxInだった場合はそのまま書き出しサイズを計算する
		if( itr->signatureScript()->scriptType() == static_cast<int>(SCRIPT_TYPE::Sig) ){
			exportRawSize += itr->exportRawSize();
		}
		else{
			// 最終的にTxInに署名が格納されていないからここで格納する
			itr->signatureScript( itr->signatureScript()->createSigScript( itr->sig() , itr->sigSize(), itr->pkey() )  ); 
			exportRawSize += itr->exportRawSize();
		}
	}
	
	return exportRawSize; }








unsigned int P2PKH::exportTxOutRaw( unsigned char **ret )
{
	*ret = new unsigned char[ exportTxOutRawSize() ];

	unsigned char* exportTxOutBuff = NULL;  unsigned int exportTxOutBuffSize = 0;
	unsigned int pos = 0;

	for( auto& itr : _body._outs )
	{
		exportTxOutBuffSize = itr->exportRaw( &exportTxOutBuff );
		memcpy( *ret + pos , exportTxOutBuff, exportTxOutBuffSize );
		pos += exportTxOutBuffSize;

		delete(exportTxOutBuff);  exportTxOutBuff = nullptr;
	}

	return exportTxOutRawSize();
}



unsigned int P2PKH::exportTxOutRawSize()
{
	unsigned int retSize = 0;

	for( auto &itr : _body._outs )
	{
		retSize += itr->exportRawSize();
	}

	return retSize;
}




unsigned int P2PKH::exportRaw( unsigned char **ret )
{
	
	//std::cout << "$ $# $ # # # # # # # # # #  #   $ $  $ $ $ $ $ $ "	 << "\n";

	//std::cout << "0 : sigSize -> " << _body._ins.at(0)->_sigSize  << "\n";
	//for( int i=0; i<_body._ins.at(0)->_sigSize ; i++){
		//printf("%02X" , _body._ins.at(0)->_sig[i]);
	//} std::cout << "\n";

	//std::cout << "1 : sigSize -> " << _body._ins.at(1)->_sigSize  << "\n";
	//for( int i=0; i<_body._ins.at(1)->_sigSize ; i++){
		//printf("%02X" , _body._ins.at(1)->_sig[i]);
	//} std::cout << "\n";

	//std::cout << "$ $# $ # # # # # # # # # #  #   $ $  $ $ $ $ $ $ "	 << "\n";
	



	if( isSigned == false )	 return 0;
	
	*ret = new unsigned char[ exportRawSize() ];
	unsigned int pos = 0;
	uint16_t InOutCnt;

	// versionの書き出し
	memcpy( *ret  , &(_body._version) , sizeof(_body._version) );  pos += sizeof(_body._version);  
	//memset( *ret , 0xff, sizeof(_body._version) );


	//= TxIn関連の書き出し =======================================================

	InOutCnt = htons( _body._ins.size() );
	memcpy(  *ret + pos , &InOutCnt ,  sizeof(InOutCnt) );      pos += sizeof(InOutCnt);

	unsigned char* exportTxInRawBuff = NULL;  unsigned int exportTxInRawBuffSize = 0;
	exportTxInRawBuffSize = exportTxInRaw( &exportTxInRawBuff );
	memcpy( *ret + pos , exportTxInRawBuff, exportTxInRawBuffSize );  pos += exportTxInRawBuffSize;
	delete exportTxInRawBuff; exportTxInRawBuff = nullptr;


	//= TxOut関連の書き出し =======================================================

	InOutCnt = htons( _body._outs.size() );
	memcpy( *ret + pos , &InOutCnt , sizeof(InOutCnt) );       pos += sizeof(InOutCnt);

	unsigned char* exportTxOutRawBuff = NULL ; unsigned int exportTxOutRawBuffSize = 0;
	exportTxOutRawBuffSize = exportTxOutRaw( &exportTxOutRawBuff );
	memcpy( *ret + pos , exportTxOutRawBuff , exportTxOutRawBuffSize ); pos += exportTxOutRawBuffSize;
	delete exportTxOutRawBuff; exportTxOutRawBuff = nullptr	;


	return exportRawSize();
}




unsigned int P2PKH::exportRawSize()
{
	if( isSigned == false ) return 0;

	unsigned int exportRawSize = 0;
	
	exportRawSize += sizeof(_body._version);
	exportRawSize += sizeof(uint16_t); // InCntのサイズ
	exportRawSize += sizeof(uint16_t); // OutCntのサイズ

	exportRawSize += exportTxInRawSize(); // TxInの全体書き出しサイズ
	exportRawSize += exportTxOutRawSize(); // TxOutの全体書き出しサイズ

	return exportRawSize;
}






unsigned char* P2PKH::txID()
{
	unsigned char* exportBuff = nullptr;  unsigned int exportBuffSize = 0;
	exportBuffSize = exportRaw( &exportBuff );

	unsigned char* txID;

	txID = hash::SHAHash( exportBuff , exportBuffSize , "sha1" );

	return txID;
}
*/






















}; // close tx namespace 
