#include "p2pkh.h"

#include "../tx/tx_in.h"
#include "../tx/tx_out.h"
#include "../script/script.h"
#include "../script/script_validator.h"
#include "../script/signature_script.h"
#include "../script/pk_script.h"
#include "../../../share/cipher/ecdsa_manager.h"
#include "../../../share/hash/sha_hash.h"


#include "../../utxo_set/utxo.h"
#include "../../utxo_set/utxo_set.h"

#include "../../validation/validation.h"

#include "openssl/bio.h" 
#include "openssl/evp.h"


namespace tx{


P2PKH::P2PKH()
{
	memset( &(_meta._version), 0x00 , sizeof(_meta._version));
}

unsigned short P2PKH::inCount()
{
	return _meta._ins.size();
}

unsigned short P2PKH::outCount()
{
	return _meta._outs.size();
}

int P2PKH::add( std::shared_ptr<TxIn> target )
{
	_meta._ins.push_back( target );
	return _meta._ins.size();
}

int P2PKH::add( std::shared_ptr<TxOut> target )
{
	_meta._outs.push_back( target );
	return _meta._outs.size();
}

std::vector< std::shared_ptr<TxIn> > P2PKH::ins()
{
	return _meta._ins;
}

std::vector< std::shared_ptr<TxOut> > P2PKH::outs()
{
	return _meta._outs;
}

unsigned int P2PKH::formatExportedRawTxVector( std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxVector, std::shared_ptr<unsigned char> *retRaw )
{
	if( exportedRawTxVector.size() <= 0 ) return 0;

	// 先ず,書き出しサイズを計算する
	unsigned int retLength = 0; 
	for( auto itr : exportedRawTxVector ) retLength += itr.second;

	if( retRaw == nullptr ) return retLength;

	// 書き出し
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
	unsigned int formatPtr = 0;
	for( auto itr : exportedRawTxVector )
	{
		memcpy( (*retRaw).get() + formatPtr , itr.first.get() , itr.second ); formatPtr += itr.second;
	}
	
	return formatPtr;
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

	for( int i=0; i<_meta._ins.size(); i++ )
	{
		for( int j=0; j<_meta._ins.size(); j++ )
		{
		
			if( i == j ) // 公開鍵ハッシュを格納する
				rawSignLength = _meta._ins.at(j)->exportRawWithPubKeyHash( &exportexRawTx );
			else // 署名スクリプトが格納されていないtx_inを書き出す
				rawSignLength = _meta._ins.at(j)->exportRawWithEmpty( &exportexRawTx );

			exportedRawTxInVector.push_back( std::make_pair(exportexRawTx,rawSignLength) ); // 生のtx_inを全て
		}
		
		for( auto itr : _meta._outs )
		{
			exportedRawTxLength = itr->exportRaw( &exportexRawTx );
			exportedRawTxOutVector.push_back( std::make_pair(exportexRawTx, exportedRawTxLength ) );
		}

		unsigned int exportedRawTxInsLength = 0; std::shared_ptr<unsigned char> exportedRawTxIns; 
		exportedRawTxInsLength = formatExportedRawTxVector( exportedRawTxInVector, &exportedRawTxIns );

		unsigned int exportedRawTxOutsLength = 0; std::shared_ptr<unsigned char> exportedRawTxOuts;
		exportedRawTxOutsLength = formatExportedRawTxVector( exportedRawTxOutVector , &exportedRawTxOuts );

		formatPtr = 0;
		unsigned int exportedRawLength; std::shared_ptr<unsigned char> exportedRaw;
		uint32_t tx_in_count = htonl(_meta._ins.size());
		uint32_t tx_out_count = htonl(_meta._outs.size());

		exportedRawLength = sizeof(_meta._version) + sizeof(tx_in_count) + exportedRawTxInsLength + sizeof(tx_out_count) + exportedRawTxOutsLength;
		exportedRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedRawLength] );

		/* [(uint32) version] 書き出し */
		memcpy( exportedRaw.get() , &(_meta._version) , sizeof(_meta._version) ); formatPtr += sizeof(_meta._version);

		/* [(uint32) tx_in_count] 書き出し */
		memcpy( exportedRaw.get() + formatPtr , &tx_in_count , sizeof(tx_in_count) ); formatPtr += sizeof(tx_in_count);

		/* [(可変長) tx_ins] 書き出し */
		memcpy( exportedRaw.get() + formatPtr , exportedRawTxIns.get() , exportedRawTxInsLength ); formatPtr += exportedRawTxInsLength;

		/* [(uint32) tx_out_count] 書き出し */
		memcpy( exportedRaw.get() + formatPtr , &tx_out_count , sizeof(tx_out_count) ); formatPtr += sizeof(tx_out_count);

		/* [(可変長) tx_outs] 書き出し */
		memcpy( exportedRaw.get() + formatPtr , exportedRawTxOuts.get() , exportedRawTxOutsLength ); formatPtr += exportedRawTxOutsLength;

		// 書き出したデータをハッシュして,秘密鍵で署名を作成する
		std::shared_ptr<unsigned char> hashedExportedRaw; unsigned int hashedExportedRawLength = 0;
		std::shared_ptr<unsigned char> encryptedExportedRaw; unsigned int encryptedExportedRawLength;
		// hashedExportedRawLength = hash::SHAHash( exportedRaw , exportedRawLength , &hashedExportedRaw , "sha256" );

		// 署名値を生成する
		signLength = cipher::ECDSAManager::sign( exportedRaw, exportedRawLength, _meta._ins.at(i)->pkey(), &sign );

		_meta._ins.at(i)->sign( sign, signLength , true ); // 配下tx_inのsignature_scriptに署名値を格納

		if( cipher::ECDSAManager::verify( sign, signLength , exportedRaw, exportedRawLength ,_meta._ins.at(i)->pkey(), "sha256" ) )
			std::cout << "\033[32m" <<  "[ TxIn(" << i << ") ] Verify Successfyly Done" << "\033[0m" << "\n";


		/* Guide Message
		std::cout << "------------------------------------------" << "\n";
		std::cout << "ExportedRaw (" << exportedRawLength << ") :: ";
		for( int i=0; i<exportedRawLength; i++ )
		{
			printf("%02X", exportedRaw.get()[i]);
		} std::cout << "\n";
		std::cout << "Generated Sign (" << signLength << ") :: ";
		for( int i=0; i<signLength; i++ )
		{
			printf("%02X", sign.get()[i]);
		} std::cout << "\n";
		std::cout << "------------------------------------------" << "\n";
		*/

		exportedRawTxInVector.clear();
		exportedRawTxOutVector.clear();
	}

	return true;
}

bool P2PKH::verify( std::shared_ptr<chain::LightUTXOSet> utxoSet )
{
	// 参照しているUTXOからlockingScriptを取り出し,tx_inのlockingScriptと結合する	
	// 全てのtx_inに対して行われる

	int i=0;
	std::shared_ptr<chain::UTXO> utxo;
	bool flag = false;
	for( auto itr : _meta._ins )
	{
		// ここでutxoからpk_scriptを取得する
		utxo = utxoSet->get( itr->prevOut() );

		if( utxo == nullptr ) return false;

		std::cout << "start p2pkh verification" << "\n";
		std::shared_ptr<unsigned char> txHash; unsigned int txHashLength;


		if( utxo->pkScript()->script()->size() <= 0 ){
			std::cout << "PkScript(locking script)がセットされていません" << "\n";
			return false;
		}
		if( itr->signatureScript()->script()->size() <= 0 ){
			std::cout << "SignatureScript(signature script)がセットされていません" << "\n";
			return false;
		}

		ScriptValidator validator( utxo->pkScript(), itr->signatureScript() );

		txHashLength = this->txHashOnTxIn( i , &txHash ); // 指定のインデックスにpubKeyHashをセットして書き出す
		flag = validator.verifyP2PKHScript( txHash, txHashLength , false );
		flag = true;
		// std::cout << "単体TxIn検証 :: " << flag << "\n";
		i++;
	}
	return flag;
}

unsigned int P2PKH::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	// 全てのtx_inの署名が住んでいることを確認する
	for( auto itr : _meta._ins )
		if( !(itr->isSigned()) ) return 0; // 一箇所でも署名が済んでいないtx_inがある場合は書き出しを行わない

	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxInVector;
	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxOutVector;
	std::shared_ptr<unsigned char> exportRawTx; unsigned int exportRawLength = 0;

	for( auto itr : _meta._ins )
	{
		exportRawLength = itr->exportRawWithSignatureScript( &exportRawTx );
		exportedRawTxInVector.push_back( std::make_pair(exportRawTx, exportRawLength) );
	}

	for( auto itr : _meta._outs )
	{
		exportRawLength = itr->exportRaw( &exportRawTx );
		exportedRawTxOutVector.push_back( std::make_pair(exportRawTx,exportRawLength) );
	}

	unsigned int exportedRawTxInsLength = 0; std::shared_ptr<unsigned char> exportedRawTxIns; 
	exportedRawTxInsLength = formatExportedRawTxVector( exportedRawTxInVector, &exportedRawTxIns );

	unsigned int exportedRawTxOutsLength = 0; std::shared_ptr<unsigned char> exportedRawTxOuts;
	exportedRawTxOutsLength = formatExportedRawTxVector( exportedRawTxOutVector , &exportedRawTxOuts );
		 
	unsigned int retRawLength = 0; unsigned int formatPtr = 0;
	uint32_t tx_in_count = htonl(_meta._ins.size());
	uint32_t tx_out_count = htonl(_meta._outs.size());
	retRawLength = sizeof(_meta._version) + sizeof(tx_in_count) + exportedRawTxInsLength + sizeof(tx_out_count) + exportedRawTxOutsLength ;
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );

	/* [(uint32) version] 書き出し */
	memcpy( (*retRaw).get() , &(_meta._version) , sizeof(_meta._version) ); formatPtr += sizeof(_meta._version);

	/* [(uint32) tx_in_count] 書き出し */
	memcpy( (*retRaw).get() + formatPtr , &tx_in_count , sizeof(tx_in_count) ); formatPtr += sizeof(tx_in_count);

	/* [(可変長) tx_ins] 書き出し */
	memcpy( (*retRaw).get() + formatPtr , exportedRawTxIns.get() , exportedRawTxInsLength ); formatPtr += exportedRawTxInsLength;

	/* [(uint32) tx_out_count] 書き出し */
	memcpy( (*retRaw).get() + formatPtr , &tx_out_count , sizeof(tx_out_count) ); formatPtr += sizeof(tx_out_count);

	/* [(可変長) tx_outs] 書き出し */
	memcpy( (*retRaw).get() + formatPtr , exportedRawTxOuts.get() , exportedRawTxOutsLength ); formatPtr += exportedRawTxOutsLength;

	//std::cout << "### " << _meta._ins.at(0)->scriptBytes() << "\n";
	//std::cout << "### " << _meta._outs.at(0)->pkScriptBytes() << "\n";

	return formatPtr;
}

size_t P2PKH::importRawSequentially( std::shared_ptr<unsigned char> fromRaw ) // 本来はちゃんとエラーハンドリングする
{
	unsigned int currentPtr = 0;

	/* versionの取り込み */
	memcpy( &(_meta._version) , fromRaw.get() , sizeof(_meta._version) ); // _versionの取り出し
	currentPtr += sizeof(_meta._version);

	/* tx_in_countの取り込み*/
	uint32_t tx_in_count;
	memcpy( &tx_in_count, fromRaw.get() + currentPtr , sizeof(tx_in_count) );
	currentPtr += sizeof(tx_in_count);

	/* tx_inの取り込み */ // 本来は種類によって取り込みを分ける
	unsigned int importedTxInLength = 0; 
	for( int i=0; i< ntohl(tx_in_count); i++ )
	{
		std::shared_ptr<TxIn> importedTxIn = std::shared_ptr<TxIn>( new TxIn );
		importedTxInLength = importedTxIn->importRaw( fromRaw.get() + currentPtr ); // ここだけunsigned char*だと不自然だけど..
		currentPtr += importedTxInLength;	
		_meta._ins.push_back( importedTxIn );
	}

	/*tx_out_countの取り込み*/
	uint32_t tx_out_count;
	memcpy( &tx_out_count, fromRaw.get() + currentPtr , sizeof(tx_out_count) );
	currentPtr += sizeof(tx_out_count);

	/* tx_outの取り込み */ // 本来は種類によって取り込みを分ける
	unsigned int importedTxOutLength = 0;
	for( int i=0; i<ntohl(tx_out_count); i++ )
	{
		std::shared_ptr<TxOut> importedTxOut = std::shared_ptr<TxOut>( new TxOut );
		
		importedTxOutLength = importedTxOut->importRaw( fromRaw.get() + currentPtr );
		currentPtr += importedTxOutLength;
		_meta._outs.push_back( importedTxOut );
	}

	return currentPtr; // 何bytesインポートしたか
}

size_t P2PKH::importRawSequentially( void *fromRaw ) // 本来はちゃんとエラーハンドリングする
{
	unsigned char *_fromRaw = static_cast<unsigned char*>(fromRaw); // ポインタインクリメントの為キャストしておく
	unsigned int currentPtr = 0;

	/* versionの取り込み */
	memcpy( &(_meta._version) , _fromRaw , sizeof(_meta._version) ); // _versionの取り出し
	currentPtr += sizeof(_meta._version);

	/* tx_in_countの取り込み*/
	uint32_t tx_in_count;
	memcpy( &tx_in_count, _fromRaw + currentPtr , sizeof(tx_in_count) );
	currentPtr += sizeof(tx_in_count);

	/* tx_inの取り込み */ // 本来は種類によって取り込みを分ける
	unsigned int importedTxInLength = 0; 
	for( int i=0; i< ntohl(tx_in_count); i++ )
	{
		std::shared_ptr<TxIn> importedTxIn = std::shared_ptr<TxIn>( new TxIn );
		importedTxInLength = importedTxIn->importRaw( _fromRaw + currentPtr ); // ここだけunsigned char*だと不自然だけど..
		currentPtr += importedTxInLength;	
		_meta._ins.push_back( importedTxIn );
	}

	/*tx_out_countの取り込み*/
	uint32_t tx_out_count;
	memcpy( &tx_out_count, _fromRaw + currentPtr , sizeof(tx_out_count) );
	currentPtr += sizeof(tx_out_count);

	
	/* tx_outの取り込み */ // 本来は種類によって取り込みを分ける
	unsigned int importedTxOutLength = 0;
	for( int i=0; i<ntohl(tx_out_count); i++ )
	{
		std::shared_ptr<TxOut> importedTxOut = std::shared_ptr<TxOut>( new TxOut );
		
		importedTxOutLength = importedTxOut->importRaw( _fromRaw + currentPtr );
		currentPtr += importedTxOutLength;
		_meta._outs.push_back( importedTxOut );
	}

	return currentPtr; // 何bytesインポートしたか
}

unsigned int P2PKH::txHashOnTxIn( int index , std::shared_ptr<unsigned char> *retRaw )
{
	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxInVector;
	std::vector< std::pair<std::shared_ptr<unsigned char>,unsigned int> > exportedRawTxOutVector;
	std::shared_ptr<unsigned char> exportexRawTx; unsigned int exportedRawTxLength;
	std::shared_ptr<unsigned char> rawSign; unsigned int rawSignLength = 0;
	std::shared_ptr<unsigned char> sign; unsigned int signLength = 0;
	unsigned int formatPtr = 0;

	for( int i=0; i<_meta._ins.size(); i++ )
	{
		
		if( i == index ) // 公開鍵ハッシュを格納する
			rawSignLength = _meta._ins.at(i)->exportRawWithPubKeyHash( &exportexRawTx );
		else
			rawSignLength = _meta._ins.at(i)->exportRawWithEmpty( &exportexRawTx );

		exportedRawTxInVector.push_back( std::make_pair(exportexRawTx,rawSignLength) ); // 生のtx_inを全て
	}
		
	for( auto itr : _meta._outs )
	{
		exportedRawTxLength = itr->exportRaw( &exportexRawTx );
		exportedRawTxOutVector.push_back( std::make_pair(exportexRawTx, exportedRawTxLength ) );
	}
		
	unsigned int exportedRawTxInsLength = 0; std::shared_ptr<unsigned char> exportedRawTxIns; 
	exportedRawTxInsLength = formatExportedRawTxVector( exportedRawTxInVector, &exportedRawTxIns );

	unsigned int exportedRawTxOutsLength = 0; std::shared_ptr<unsigned char> exportedRawTxOuts;
	exportedRawTxOutsLength = formatExportedRawTxVector( exportedRawTxOutVector , &exportedRawTxOuts );

	formatPtr = 0;
	unsigned int exportedRawLength; std::shared_ptr<unsigned char> exportedRaw;
	uint32_t tx_in_count = htonl(_meta._ins.size());
	uint32_t tx_out_count = htonl(_meta._outs.size());

	exportedRawLength = sizeof(_meta._version) + sizeof(tx_in_count) + exportedRawTxInsLength + sizeof(tx_out_count) + exportedRawTxOutsLength;
	exportedRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedRawLength] );

		/* [(uint32) version] 書き出し */
	memcpy( exportedRaw.get() , &(_meta._version) , sizeof(_meta._version) ); formatPtr += sizeof(_meta._version);

		/* [(uint32) tx_in_count] 書き出し */
	memcpy( exportedRaw.get() + formatPtr , &tx_in_count , sizeof(tx_in_count) ); formatPtr += sizeof(tx_in_count);

		/* [(可変長) tx_ins] 書き出し */
	memcpy( exportedRaw.get() + formatPtr , exportedRawTxIns.get() , exportedRawTxInsLength ); formatPtr += exportedRawTxInsLength;

		/* [(uint32) tx_out_count] 書き出し */
	memcpy( exportedRaw.get() + formatPtr , &tx_out_count , sizeof(tx_out_count) ); formatPtr += sizeof(tx_out_count);

		/* [(可変長) tx_outs] 書き出し */
	memcpy( exportedRaw.get() + formatPtr , exportedRawTxOuts.get() , exportedRawTxOutsLength ); formatPtr += exportedRawTxOutsLength;

	*retRaw = exportedRaw;

	return exportedRawLength;
}

unsigned int P2PKH::calcTxID( std::shared_ptr<unsigned char> *ret )
{
	std::shared_ptr<unsigned char> exportedP2PKH; unsigned int exportedP2PKHLength;
	exportedP2PKHLength = this->exportRaw( &exportedP2PKH );
	
	return hash::SHAHash( exportedP2PKH, exportedP2PKHLength , ret , "sha256" );
}

std::vector<std::uint8_t> P2PKH::export_to_binary()
{
  
  // 仮の実装 後で修正する
  unsigned int p2pkh_binary_length; 
  std::shared_ptr<unsigned char> p2pkh_binary;
  p2pkh_binary_length = this->exportRaw( &p2pkh_binary );

  std::vector<std::uint8_t> ret;
  ret.resize( p2pkh_binary_length );
  std::memcpy( ret.data(), p2pkh_binary.get(), p2pkh_binary_length );
  return ret;
}

/*
int P2PKH::TxInCnt()
{
	return _meta._ins.size(); }


int P2PKH::TxOutCnt() // getter
{
	return _meta._outs.size();
}



// raw to structure
P2PKH::P2PKH( unsigned char* rawP2PKHBuff , unsigned int rawP2PKHBuffSize )
{

	unsigned int pos = 0;

	memcpy( &(_meta._version) , rawP2PKHBuff , sizeof(_meta._version) );  pos += sizeof(_meta._version);  // versionの取り込み

	uint16_t InOutCnt;
	
	// = Ins の取り込み ================================================================= 
	// ins count 
	memcpy( &InOutCnt  ,rawP2PKHBuff  + pos , sizeof(InOutCnt) );  pos += sizeof(InOutCnt); 

	std::cout << "-> " << ntohs( InOutCnt ) << "\n";
	std::cout << "--> " << ntohs( InOutCnt ) << "\n";

	unsigned char *rawprev_outBuff = nullptr;
	TxIn *newTxIn;

	for( int i=0; i < ntohs(InOutCnt) ; i++ )
	{							
		newTxIn = new TxIn;

		// なぜかこれを追加すると　TxIn->signatureScript が書き換えられる　  // ヒープ領域のメモリ不足で勝手に解放しちゃってる？
		 // memcpy( rawprev_outBuff, rawP2PKHBuff + pos  , PREV_OUT_SIZE );  pos += PREV_OUT_SIZE;
		//newTxIn->prevOut( rawprev_outBuff, PREV_OUT_SIZE );
		//delete rawprev_outBuff; rawprev_outBuff = nullptr;
	
		pos += newTxIn->autoTakeInSignatureScript( rawP2PKHBuff + pos ); // 戻り値が正常に取り込んだsignatureScriptのサイズになる
		_meta._ins.push_back( newTxIn );
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
		_meta._outs.push_back( newTxOut );

		// 本来はscript_bytesまでもscriptで持たせてtakeInScriptメソッドで済ませた方がまとまりが良い
	}
	// outsの取り込み完了 ==========================================================================

	std::cout << "Take In TxOut from RawP2PKH DONE" << "\n";

	return;
}


//cher up Yuri San, 加油小弟、がんばれカッパくん。 2023.4.9 -kushima


void P2PKH::addInput( TxIn *txIn )
{
	_meta._ins.push_back( txIn );
}


void P2PKH::addOutput( TxOut* txOut )
{
	_meta._outs.push_back( txOut );
}





bool P2PKH::sign()
{

	//unsigned char* sig = NULL;
	//unsigned int sigSize = 0;

	unsigned char* partialBuff = NULL;
	unsigned int partialBuffSize = 0;

	unsigned char* sigBuff; unsigned int sigBuffSize = 0;

	//int i=0;
 	//for (auto& itr : _meta._ins ) 
	for( int i=0; i< _meta._ins.size(); i++)	
	{
		partialBuffSize = exportPartialRaw( &partialBuff , i ); // 署名用に該当するTxにDestPubKeyをそれ以外は空白にしたTxInにしてトランザクション全体を書き出す		

		sigBuffSize =  cipher::ECDSAManager::Sign( partialBuff , partialBuffSize , &sigBuff , _meta._ins.at(i)->pkey() );

		_meta._ins.at(i)->sig( sigBuff );
		_meta._ins.at(i)->sigSize(  sigBuffSize ) ;

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
	for( auto& itr : _meta._ins ){

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


	for( int i=0 ; i < _meta._ins.size() ; i++ ){

		// なぜ最初からvector(_script)に要素が入っているのか？ -> create関数の中でclearすることで一時的に対応	

		if( i == index ) // 目的のInxに到達した公開鍵ハッシュをセットする
		{
			_meta._ins.at(i)->signatureScript( _meta._ins.at(i)->signatureScript()->createHashedPubKeyScript( _meta._ins.at(i)->pkey()) );
			exportTxInBuffSize = _meta._ins.at(i)->exportRaw( &exportTxInBuff );
		}
		else{
			exportTxInBuffSize = _meta._ins.at(i)->exportEmptyRaw( &exportTxInBuff ); // signature script empty tx in
		}

		memcpy( *ret + pos , exportTxInBuff , exportTxInBuffSize ); pos += exportTxInBuffSize;

		delete exportTxInBuff;   exportTxInBuff = nullptr;
	}

	return exportPartialTxInSize( index );
};




unsigned int P2PKH::exportPartialTxInSize( int index ){

	unsigned int exportTxInSize = 0;

	for( int i=0; i<_meta._ins.size(); i++ )
	{
		if( i == index ){
			_meta._ins.at(i)->signatureScript( _meta._ins.at(i)->signatureScript()->createHashedPubKeyScript( _meta._ins.at(i)->pkey() ) );
			exportTxInSize += _meta._ins.at(i)->exportRawSize();
		}
		else{
			exportTxInSize += _meta._ins.at(i)->exportEmptyRawSize();
		}
	}
	return exportTxInSize;
}






unsigned int P2PKH::exportPartialRaw( unsigned char **ret , int partialTxInIndex )
{	
	*ret = new unsigned char[ exportPartialRawSize( partialTxInIndex ) ];

	unsigned int pos = 0;
	uint16_t InOutCnt;

	memcpy( *ret  , &(_meta._version) , sizeof(_meta._version) ); pos += sizeof(_meta._version); // versionの書き出し


	// = TxIn関連の書き出し =======================================================

	// カウントの書き出し
 	InOutCnt = htons( _meta._ins.size() );
	memcpy(  *ret + pos , &InOutCnt ,  sizeof(InOutCnt) ); pos += sizeof(InOutCnt);
	// TxIns の書き出し 
	unsigned char* exportPartialTxInRawBuff; unsigned int exportPartialTxInRawBuffSize = 0;
	exportPartialTxInRawBuffSize = exportPartialTxIn( &exportPartialTxInRawBuff , partialTxInIndex );
	memcpy( *ret + pos , exportPartialTxInRawBuff , exportPartialTxInRawBuffSize );  pos += exportPartialTxInRawBuffSize;




	// = TxOut関連の書き出し =======================================================

	// カウントの書き出し
	InOutCnt = htons( _meta._outs.size() );
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
	return sizeof(_meta._version) + sizeof(uint16_t) + sizeof(uint16_t) + exportPartialTxInSize( partialTxInIndex ) + exportTxOutRawSize();
}






unsigned int P2PKH::exportTxInRaw( unsigned char **ret )
{		
	*ret = new unsigned char[ exportTxInRawSize() ];

	unsigned char* exportTxInBuff = NULL; unsigned int exportTxInBuffSize = 0;
	unsigned int pos = 0;

	//unsigned char* sigBuff = nullptr; unsigned int sigBuffSize = 0;


	int i=0;
	for( auto &itr : _meta._ins ){

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

	for( auto &itr : _meta._ins ){

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

	for( auto& itr : _meta._outs )
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

	for( auto &itr : _meta._outs )
	{
		retSize += itr->exportRawSize();
	}

	return retSize;
}




unsigned int P2PKH::exportRaw( unsigned char **ret )
{
	
	//std::cout << "$ $# $ # # # # # # # # # #  #   $ $  $ $ $ $ $ $ "	 << "\n";

	//std::cout << "0 : sigSize -> " << _meta._ins.at(0)->_sigSize  << "\n";
	//for( int i=0; i<_meta._ins.at(0)->_sigSize ; i++){
		//printf("%02X" , _meta._ins.at(0)->_sig[i]);
	//} std::cout << "\n";

	//std::cout << "1 : sigSize -> " << _meta._ins.at(1)->_sigSize  << "\n";
	//for( int i=0; i<_meta._ins.at(1)->_sigSize ; i++){
		//printf("%02X" , _meta._ins.at(1)->_sig[i]);
	//} std::cout << "\n";

	//std::cout << "$ $# $ # # # # # # # # # #  #   $ $  $ $ $ $ $ $ "	 << "\n";
	



	if( isSigned == false )	 return 0;
	
	*ret = new unsigned char[ exportRawSize() ];
	unsigned int pos = 0;
	uint16_t InOutCnt;

	// versionの書き出し
	memcpy( *ret  , &(_meta._version) , sizeof(_meta._version) );  pos += sizeof(_meta._version);  
	//memset( *ret , 0xff, sizeof(_meta._version) );


	//= TxIn関連の書き出し =======================================================

	InOutCnt = htons( _meta._ins.size() );
	memcpy(  *ret + pos , &InOutCnt ,  sizeof(InOutCnt) );      pos += sizeof(InOutCnt);

	unsigned char* exportTxInRawBuff = NULL;  unsigned int exportTxInRawBuffSize = 0;
	exportTxInRawBuffSize = exportTxInRaw( &exportTxInRawBuff );
	memcpy( *ret + pos , exportTxInRawBuff, exportTxInRawBuffSize );  pos += exportTxInRawBuffSize;
	delete exportTxInRawBuff; exportTxInRawBuff = nullptr;


	//= TxOut関連の書き出し =======================================================

	InOutCnt = htons( _meta._outs.size() );
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
	
	exportRawSize += sizeof(_meta._version);
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
