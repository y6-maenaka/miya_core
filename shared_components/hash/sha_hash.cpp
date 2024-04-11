#include "sha_hash.h"

namespace hash{

// templateを使った方がいい？
unsigned char* SHAHash( unsigned char* HashMessage, size_t HashMessageSize , char* SHAType){

	const EVP_MD *md; // MessageDigest
	size_t HashSize;
	unsigned char* Ret;

	EVP_MD_CTX *mdctx = EVP_MD_CTX_create(); // MessageDigest Context の新規作成と初期化 下コメントを簡略化した処理
	md = EVP_get_digestbyname( SHAType );

	Ret = (unsigned char *)malloc( EVP_MD_size(md) );


	EVP_DigestInit_ex( mdctx, md, NULL ); // 第3引数は暗号エンジン
	EVP_DigestUpdate( mdctx, HashMessage, HashMessageSize);
	EVP_DigestFinal_ex( mdctx, Ret, (unsigned int *)&HashMessageSize );


	// 非推奨 EVP_DigestFinalで自動的に削除される
	//EVP_MD_CTX_cleanup( mdctx );
	
	return Ret;
}





size_t SHAHash( std::shared_ptr<unsigned char> targetMSG , size_t targetMSGLength, std::shared_ptr<unsigned char> *ret ,char* SHAtype )
{
	const EVP_MD *md; // MessageDigest
	size_t HashSize;
	unsigned int retLength;
	
	EVP_MD_CTX *mdctx = EVP_MD_CTX_create(); // MessageDigest Context の新規作成と初期化 下コメントを簡略化した処理

	md = EVP_get_digestbyname( SHAtype );

	*ret = std::shared_ptr<unsigned char>( new unsigned char[EVP_MD_size(md)] );

	EVP_DigestInit_ex( mdctx, md, NULL ); // 第3引数は暗号エンジン
	EVP_DigestUpdate( mdctx, targetMSG.get(), targetMSGLength );
	EVP_DigestFinal_ex( mdctx, (*ret).get() , (unsigned int *)&retLength );

	return retLength;
}


size_t SHAHash( unsigned char* targetMSG , size_t targetMSGLength , std::shared_ptr<unsigned char> *ret , char* SHAtype )
{
   const EVP_MD *md; // MessageDigest
	size_t HashSize;
	unsigned int retLength;
	
	EVP_MD_CTX *mdctx = EVP_MD_CTX_create(); // MessageDigest Context の新規作成と初期化 下コメントを簡略化した処理

	md = EVP_get_digestbyname( SHAtype );

	*ret = std::shared_ptr<unsigned char>( new unsigned char[EVP_MD_size(md)] );

	EVP_DigestInit_ex( mdctx, md, NULL ); // 第3引数は暗号エンジン
	EVP_DigestUpdate( mdctx, targetMSG , targetMSGLength );
	EVP_DigestFinal_ex( mdctx, (*ret).get() , (unsigned int *)&retLength );

	return retLength;
}





void FreeDigest( void* target )
{
	free( target );
}





}; // close hash namesapce

int main(){
	unsigned char Message[] = "Hello";
	unsigned char* HashedMessage;
	HashedMessage = hash::SHAHash( Message, 5 , DIGEST_NAME_SHA1);

	for(int i=0 ; i<20; i++){
		printf("%02X", HashedMessage[i]);
	}
}



