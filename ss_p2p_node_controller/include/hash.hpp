#ifndef F5E6A668_45FD_4E12_B9A1_4EB1F139EE77
#define F5E6A668_45FD_4E12_B9A1_4EB1F139EE77


#include <memory>
#include <vector>
#include <span>
#include <algorithm>

#include "openssl/evp.h"


namespace ss
{


constexpr unsigned short SHA1_HASH_LENGTH = (160/8);


template< typename T >
static std::vector<unsigned char> sha1_hash( T* bin, std::size_t bin_len ) // 無駄なSTL変換がある
{
  std::shared_ptr<unsigned char> in = std::shared_ptr<unsigned char>( new unsigned char[bin_len] );
  // std::memcpy( in.get(), bin, bin_len );
  memcpy( in.get(), bin, bin_len ); // binがコンパイルエラー出す

  const EVP_MD *md;
  std::size_t out_len = 0;

  EVP_MD_CTX *md_ctx = EVP_MD_CTX_create();
  md = EVP_get_digestbyname( "sha1" );
  std::shared_ptr<unsigned char> out = std::shared_ptr<unsigned char>( new unsigned char[EVP_MD_size(md)] );

  if( EVP_DigestInit_ex( md_ctx , md, nullptr ) <= 0 )
  {
	EVP_MD_CTX_free( md_ctx );
	return std::vector<unsigned char>();
  }

  if( EVP_DigestUpdate( md_ctx, in.get() , bin_len ) <= 0 )
  {
	EVP_MD_CTX_free( md_ctx );
	return std::vector<unsigned char>();
  }

  if( EVP_DigestFinal_ex( md_ctx , out.get(),  (unsigned int *)&out_len ) <= 0 )
  {
	EVP_MD_CTX_free( md_ctx );
	return std::vector<unsigned char>();
  }

  EVP_MD_CTX_free( md_ctx );

  return std::vector<unsigned char>( out.get(), out.get() + out_len );
}


};


#endif 


