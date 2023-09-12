#include "tx_in.h"

#include "./common.h"
#include "../script/signature_script.h"
#include "../../../shared_components/cipher/ecdsa_manager.h"


namespace tx{

PrevOut::PrevOut(){


	// dummy 
	const char* id = "aaaaaaaaaabbbbbbbbbb";
	_txID = (unsigned char *)id;
	_index = htonl( 10 );
	return;
}



unsigned int PrevOut::exportRaw( unsigned char **ret )
{	

	unsigned int pos = 0;
	*ret = new unsigned char[ exportRawSize() ];

	memcpy( *ret , _txID , TX_ID_SIZE );       pos += TX_ID_SIZE;
	memcpy( *ret + pos , &_index , sizeof(_index) );  // pos += sizeof(_index);

	return exportRawSize();
}


unsigned int PrevOut::exportRawSize()
{
	return TX_ID_SIZE + sizeof( _index ) ;
}
























TxIn::TxIn() : _sig(nullptr) , _sigSize(0)
{ 
	memset( &_sequence , 0xff , sizeof(_sequence) );
	//_signatureScript = NULL;

	SignatureScript *signatureScript = new SignatureScript;
	this->_signatureScript = signatureScript;


	return;
}



unsigned char* TxIn::sig()
{
	return _sig;	
}
											
										
void TxIn::sig( unsigned char *sig )
{
	_sig = sig;
}




unsigned int TxIn::sigSize(){
	return _sigSize;
}



PrevOut *TxIn::prevOut()
{
	return _prevOut;
}


void TxIn::prevOut( unsigned char *prevOut , unsigned int prevOutSize )
{
	memcpy( _prevOut, prevOut , prevOutSize );
}




unsigned int TxIn::autoTakeInSignatureScript( unsigned char* from )
{
	unsigned int pos = 0;

	// 確かにsignatureScriptからfromがスタートしていないと合わなくなる
	memcpy( &_script_bytes , from , sizeof(_script_bytes) ); pos += sizeof(_script_bytes); // OK

	pos += _signatureScript->takeInScript( from + pos  , scriptBytes()) ;

	pos += sizeof(_sequence); // あとでちゃんと書き直す

	/*
	 ここで取り出す？
	 */

	_pkey = _signatureScript->pubKey();
	//_pkey = cipher::ECDSAManager::_myPkey;
	_sigSize = _signatureScript->sig( &_sig );


	return pos;
}



unsigned int TxIn::scriptBytes()
{
	return ntohl( _script_bytes );
}


void TxIn::sigSize( unsigned int sigSize )
{
	_sigSize = sigSize;
}





EVP_PKEY* TxIn::pkey()
{
	return _pkey;
}



void TxIn::pkey( EVP_PKEY* pkey )
{
	_pkey = pkey;
}





SignatureScript *TxIn::signatureScript()
{
	return _signatureScript;
}




void TxIn::signatureScript( SignatureScript *signatureScript )
{
	_signatureScript = signatureScript;
}



unsigned int TxIn::exportEmptyRaw( unsigned char **ret )
{
	// signature_scriptを空にして書き出し
	*ret = new unsigned char[ exportEmptyRawSize() ];

	unsigned char* prevOutBuff;   unsigned int prevOutBuffSize = 0;

	// dummyで仮置き
	_prevOut = new PrevOut;

	prevOutBuffSize = _prevOut->exportRaw( &prevOutBuff );

	unsigned int pos = 0;
	memcpy( *ret , prevOutBuff, prevOutBuffSize ); pos += prevOutBuffSize;
	//memcpy( *ret + pos , &_script_bytes , sizeof( _script_bytes) ); pos += sizeof(_script_bytes);
	memcpy( *ret + pos , &_sequence , sizeof(_sequence) );   // pos += sizeof(_sequence);

	return exportEmptyRawSize();
}





unsigned int TxIn::exportEmptyRawSize()
{
	// signature_scriptを空にして書き出し
	return ( _prevOut->exportRawSize() + sizeof(_sequence) );
}






unsigned int TxIn::exportRaw( unsigned char **ret )
{
	*ret = new unsigned char[ exportRawSize() ];


	/* = PrevOut の書き出し ================================================== */
	unsigned char* prevOutBuff = NULL; unsigned int prevOutBuffSize = 0;

	// dummyで仮置き	
	_prevOut = new PrevOut; 
														
	prevOutBuffSize = _prevOut->exportRaw( &prevOutBuff );
	/* =====================================================================  */


	/* = SignatureScript の書き出し ================================================== */
	unsigned char *signatureScriptBuff = NULL; unsigned int signatureScriptBuffSize = 0;
	signatureScriptBuffSize =  _signatureScript->exportRaw( &signatureScriptBuff );
	_script_bytes = htonl( signatureScriptBuffSize ); // スクリプトサイズのセット
	/* =====================================================================  */
	

	unsigned int pos = 0;
	memcpy( *ret , prevOutBuff , prevOutBuffSize );  pos += prevOutBuffSize;
	memcpy( *ret + pos , &_script_bytes , sizeof(_script_bytes) ); pos += sizeof(_script_bytes);
	memcpy( *ret + pos , signatureScriptBuff , signatureScriptBuffSize );  pos += signatureScriptBuffSize;
	memcpy( *ret + pos , &_sequence , sizeof(_sequence) );  pos += sizeof(_sequence);

	return exportRawSize();
}





unsigned int TxIn::exportRawSize()
{

	return _prevOut->exportRawSize() + sizeof(_script_bytes) + _signatureScript->exportRawSize() +  sizeof(_sequence);
}



std::shared_ptr<unsigned char> TxIn::inTxID()
{
	return std::make_shared<unsigned char>( *(_prevOut->_txID) );
}


unsigned short TxIn::inIndex()
{
	return static_cast<unsigned short>(_prevOut->_index);
}






}; // close tx namespace
