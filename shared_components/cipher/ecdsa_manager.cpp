#include "ecdsa_manager.h"


namespace cipher{

EVP_PKEY *ECDSAManager::_myPkey = nullptr;
unsigned char* ECDSAManager::_pemPass = NULL;
unsigned int ECDSAManager::_pemPassSize = 0;



bool ECDSAManager::init( unsigned char* pemPass , unsigned int passSize ){

	if( pemPass == NULL || passSize <= 0 ){
		std::cout << "PASSWORDを設定してください" << "\n";
		return false;
	}

	setPemPass( pemPass, passSize );

	_myPkey = readPriKey( _pemPass );

	if( _myPkey == NULL ) 
	{
		char yn; // yes or no
		/* 既存のpemファイルを読み込めない場合 */
		std::cout << "鍵ファイルが見つかりません\n\n新規作成 [y]\n" << "取り消し [n]\n"  << " -> ";
		std::cin >> yn;
if( yn == 'y' ){ saveNewPkey( _pemPass , _pemPassSize ); _myPkey = readPriKey( _pemPass ); }else{
			exit(1);
		}

	}

	return true;
}




void ECDSAManager::error( const char* errorMSG , bool isExit )
{
	fprintf( stderr , "[ ECDSA ERROR ] %s ", errorMSG );

	if( isExit == true ) exit(1);
}


EVP_PKEY* ECDSAManager::myPkey()
{
	return _myPkey;
}




void ECDSAManager::setPemPass( unsigned char* pemPass , unsigned int passSize )
{
	_pemPass = pemPass;
	_pemPassSize = passSize;
}
																														






EVP_PKEY* ECDSAManager::generatePkey()
{
	EVP_PKEY_CTX* pctx = NULL, *gen_pctx = NULL; 
	EVP_PKEY *pkey = NULL, *params = NULL; // NULLで初期化しないとエラー落ちする

	// contextをECで初期化する
	pctx = EVP_PKEY_CTX_new_id( EVP_PKEY_EC, NULL );

	if( pctx == NULL )
		error("pctx is NULL");

	// パラメータを初期化	
	if( EVP_PKEY_paramgen_init( pctx ) <= 0 )
		error("paramgen_init failure");


	// コンテキストに使う曲線関数をセットする
	if( EVP_PKEY_CTX_set_ec_paramgen_curve_nid( pctx, NID_secp256k1) <= 0 )
		error("set ec paramgen curve is failure");

	if( EVP_PKEY_paramgen( pctx, &params ) <= 0 )  // 上まででセットしたパラメータをセットする
		error("paramgen is failure");


	gen_pctx = EVP_PKEY_CTX_new( params, NULL ); // 鍵生成用のコンテキストを用意する
																								
	if( gen_pctx == NULL )	
		error("gen_pctx is NULL");

	// 初期化
	if( EVP_PKEY_keygen_init( gen_pctx ) <= 0 )
		error("keygen init is failure");

	if( EVP_PKEY_keygen( gen_pctx, &pkey ) <= 0 )
		error("keygen is failure");	

	// class member にセット
	// _myPkey = pkey;

	// ctxの解放
	EVP_PKEY_CTX_free( pctx );  EVP_PKEY_CTX_free( gen_pctx ); 

	return pkey;
}





EVP_PKEY *ECDSAManager::readPubKey( const char *path )
{

	EVP_PKEY* pkey = NULL;
	BIO *read_bio_fp = NULL;
	read_bio_fp = BIO_new( BIO_s_file() );

	read_bio_fp = BIO_new_file( path ,"r");

	if( read_bio_fp == NULL ){
		std::cout << "Error Here" << "\n";
		// エラー落ちさせる
	}

	pkey = PEM_read_bio_PUBKEY( read_bio_fp , NULL, NULL, NULL );

	BIO_vfree( read_bio_fp );

	
	return pkey;
}



EVP_PKEY *ECDSAManager::readPriKey( unsigned char *pemPass ,const char *path )
{
	if( pemPass == NULL ) return NULL;

	EVP_PKEY *pkey = NULL;

	BIO *read_bio_fp = NULL;
	read_bio_fp = BIO_new( BIO_s_file() );
	read_bio_fp = BIO_new_file( path, "r" );

	if( read_bio_fp == NULL ){
		std::cout << "Error Here" << "\n";
		// エラー落ちさせる
		return NULL;
	}

	// Passがある時
	if( pemPass ){
		pkey = PEM_read_bio_PrivateKey( read_bio_fp , NULL, NULL, (char *)pemPass );
	}else{
		pkey = PEM_read_bio_PrivateKey( read_bio_fp , NULL, NULL, NULL );
	}

	BIO_vfree( read_bio_fp );

	return pkey;
}













// pubkeyは鍵しで保存
bool ECDSAManager::writePubKey( EVP_PKEY *pkey , const char* path ){

	if( pkey == NULL ) return false;

	BIO *write_bio_fp;
	write_bio_fp = BIO_new( BIO_s_file() );
	write_bio_fp = BIO_new_file( path , "w" );

	PEM_write_bio_PUBKEY( write_bio_fp , pkey );

	// bioの解放
	BIO_vfree( write_bio_fp );

	return true;
}






bool ECDSAManager::writePriKey( EVP_PKEY* pkey , const char* path , unsigned char* pemPass, unsigned int passSize )
{

	if( _pemPass == NULL || _pemPassSize <= 0 ){
		std::cout << "パスワードを設定してください" << "\n";
		return false;
	}

	if( pkey == NULL ){
		std::cout << "鍵を設定してください" << "\n";
		return false;
	}

	BIO* write_bio_fp;
	write_bio_fp = BIO_new( BIO_s_file() );
	write_bio_fp = BIO_new_file( path , "w" );
	

	// 一応 鍵なしで書き込ませることはしない
	if( _pemPass || _pemPassSize > 0 ){
		// pass付きでPemに鍵を書き込む
		PEM_write_bio_PKCS8PrivateKey( write_bio_fp , pkey , EVP_des_ede3_cbc(), (const char *)pemPass , passSize , NULL , NULL);

	}else{
		PEM_write_bio_PKCS8PrivateKey( write_bio_fp , pkey , EVP_des_ede3_cbc(), NULL, 0 , NULL , NULL);
	}

	BIO_vfree( write_bio_fp );

	return true;
}



/*
bool ECDSAManager::saveNewKeyPair( unsigned char* pemPass , unsigned int passSize , const char *pubPath , const char *priPath )
{
	EVP_PKEY* pkey = NULL;
	pkey = generateKeyPair();

	setPemPass( pemPass, passSize );

	writePubKey( pkey );
	writePriKey( pkey );

	return true;
}
*/


bool ECDSAManager::saveNewPkey( unsigned char* pemPass , unsigned int passSize )
{
	EVP_PKEY* pkey = NULL;
	pkey = generatePkey();

	setPemPass( pemPass, passSize );

	writePubKey( pkey );
	writePriKey( pkey );

	return true;

}




void ECDSAManager::printPkey( EVP_PKEY *pkey )
{
	
	if( pkey == nullptr ) return;

	BIO *output_bio = BIO_new_fp( stdout, 0 );

	// error happen here 
	EVP_PKEY_print_private( output_bio , pkey , 0 , NULL );
	EVP_PKEY_print_public(output_bio, pkey, 0, NULL);
	
	BIO_vfree( output_bio );
	
}





int ECDSAManager::toRawPubKey( unsigned char **ret , EVP_PKEY* pkey )
{
	/*
	EC_KEY *ec_key = NULL;

	ec_key = EVP_PKEY_get1_EC_KEY( pkey );

	int retLen;
	unsigned char *p;

	retLen = i2o_ECPublicKey( ec_key, NULL );
	p = (unsigned char *)malloc( retLen );
	memset( p, 0x0, retLen );


	*ret = p;
	i2o_ECPublicKey( ec_key , ret );
	
	*ret = p; // 不明点

	EC_KEY_free( ec_key );
	return retLen;
	*/

	unsigned char* rawPubKeyBuff = nullptr; unsigned int rawPubKeyBuffSize = 0;

	rawPubKeyBuffSize = i2d_PUBKEY( pkey , &rawPubKeyBuff );

	*ret = rawPubKeyBuff;

	return rawPubKeyBuffSize;
}



unsigned int ECDSAManager::toRawPubKey( EVP_PKEY *pkey , std::shared_ptr<unsigned char> *ret )
{
	unsigned char *output = nullptr; unsigned int outputLength;

	outputLength = i2d_PUBKEY( pkey ,  &output );

	*ret = std::shared_ptr<unsigned char>( output , [](unsigned char* ptr){
		if( ptr ) OPENSSL_free(ptr);
	});

	return outputLength;
}





EVP_PKEY* ECDSAManager::toPkey( unsigned char* rawPubKey , unsigned int rawPubKeySize )
{
	EVP_PKEY* pkey = NULL; 
	const unsigned char* _rawPubKey = rawPubKey;

	pkey = d2i_PUBKEY( NULL , &_rawPubKey , rawPubKeySize );
	


	return pkey;
}



/*
EVP_PKEY* ECDSAManager::toPKEY( std::shared_ptr<unsigned char> from , unsigned int fromLength )
{
	EVP_PKEY *pkey;
	//unsigned char *rawFrom = from.get();
	const unsigned char* rawFrom = from.get();
	d2i_PUBKEY( &pkey , &rawFrom, fromLength  );

	return pkey;
}
*/



int ECDSAManager::Sign( unsigned char *target, size_t targetLen , unsigned char** sig, EVP_PKEY* pri_key )
{

		size_t mdLen = 256 / 8; // あとでちゃんとしたところに書き直す
		size_t sigLen;

		// 基本的にopenssl signはinのハッシュを想定していないため事前にハッシュしておく必要がある
		unsigned char* md = NULL; // SHAHash()内部でmallocしてくれる
		md = hash::SHAHash( target, targetLen ,DIGEST_NAME_SHA256); // この手順はSign関数内部でやってくれないのか？
		if( !md ) return -1;

		EVP_PKEY_CTX *sctx = NULL;

	
		sctx = EVP_PKEY_CTX_new( pri_key , NULL ); // 鍵からCTXを初期化

		// 署名用にctxを初期化
		EVP_PKEY_sign_init( sctx );

		EVP_PKEY_sign( sctx, NULL, &sigLen , md, mdLen );


		*sig = (unsigned char *)OPENSSL_malloc( sigLen );
		memset( *sig , 0x0 , sigLen ); //　念の為
																	 
		if( !(*sig) ) return -1;

		EVP_PKEY_sign( sctx, *sig, &sigLen, (unsigned char *)md, mdLen );

		EVP_PKEY_CTX_free( sctx );

		return sigLen;
}





unsigned int ECDSAManager::sign( std::shared_ptr<unsigned char> target , unsigned int targetLength, EVP_PKEY *pkey , std::shared_ptr<unsigned char> *retSign )
{
	size_t sigLen;

	// 対象をハッシュ化する
	std::shared_ptr<unsigned char> targetDigest; unsigned int targetDigestLength = 0;
	targetDigestLength = hash::SHAHash( target , targetLength , &targetDigest , "sha256" );
	if( targetDigestLength <= 0 || targetDigest == nullptr ) return 0;

	EVP_PKEY_CTX *sctx = NULL;
	sctx = EVP_PKEY_CTX_new( pkey, NULL );

	EVP_PKEY_sign_init( sctx );
	EVP_PKEY_sign( sctx , NULL , &sigLen , targetDigest.get() , targetDigestLength ); // 署名値のサイズを取得する

	*retSign = std::shared_ptr<unsigned char>( new unsigned char[sigLen] ); memset( (*retSign).get() , 0x00 , sigLen );

	if( retSign == nullptr )
	{
		EVP_PKEY_CTX_free( sctx );
		return 0;
	}

	//sigLen = 64;
	EVP_PKEY_sign( sctx , (*retSign).get() , &sigLen , targetDigest.get() , targetDigestLength );

	EVP_PKEY_CTX_free( sctx );

	return sigLen;
}







bool ECDSAManager::Verify( unsigned char *sig, size_t sigLen , unsigned char *target, size_t targetLen, EVP_PKEY* pub_key )
{
		EVP_PKEY_CTX* vctx = NULL;
		vctx = EVP_PKEY_CTX_new( pub_key , NULL );

		unsigned char* md = NULL;
		size_t mdLen = 256 / 8;
		md = hash::SHAHash( target, targetLen, DIGEST_NAME_SHA256 );
		if( !md ) return -1;

		//BIO *bio = BIO_new_fp( stdout, 0 );
		//EVP_PKEY_print_private( bio, pub_key , 0, NULL);

		EVP_PKEY_verify_init( vctx );


		// 検証が成功したらtrue
		int ret = EVP_PKEY_verify( vctx, (const unsigned char *)sig, sigLen, (unsigned char *)md, mdLen );
		
		std::cout << "[ VERIFY ] " << ret << "\n";
		 
		if( ret == 1 ){
			EVP_PKEY_CTX_free( vctx );
			return true;
		}

		EVP_PKEY_CTX_free( vctx );

		return false;
}



bool ECDSAManager::verify( std::shared_ptr<unsigned char> sig , size_t sigLength , std::shared_ptr<unsigned char> msg , size_t msgLength , EVP_PKEY *pubKey , char *hashType )
{
	std::shared_ptr<unsigned char> md; unsigned int mdLength;
	mdLength = hash::SHAHash( msg , msgLength , &md , hashType );


	return verify( sig , sigLength , md, mdLength ,pubKey );
}


bool ECDSAManager::verify( std::shared_ptr<unsigned char> sig , size_t sigLength , std::shared_ptr<unsigned char> msgDigest , size_t msgDigestLength , EVP_PKEY *pubKey )
{
	EVP_PKEY_CTX *vctx = NULL;
	vctx = EVP_PKEY_CTX_new( pubKey , NULL );

	if( !msgDigest ) return false;

	EVP_PKEY_verify_init( vctx );

	int ret = EVP_PKEY_verify( vctx , (const unsigned char *)(sig.get()) , sigLength , (unsigned char *)(msgDigest.get()) , msgDigestLength );

	EVP_PKEY_CTX_free( vctx );

	return (ret == 1);
}






/*
bool ECDSAManager::writePkey( const char *path , EVP_PKEY *pkey )
{
	FILE *writeTargetFP = fopen( path , pkey );
	if( !writeTargetFP ) return false;


}



EVP_PKEY* ECDSAManager::readPkey( const char *path )
{

}
*/


																														
																														
}; // close cipher namespace
