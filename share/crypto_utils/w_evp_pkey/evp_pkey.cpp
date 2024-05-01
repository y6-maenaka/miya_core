#include "evp_pkey.hpp"


namespace cu
{


w_evp_pkey::w_evp_pkey()
{
  return;
}

w_evp_pkey::w_evp_pkey( std::string pemPath )
{
  return;
}

w_evp_pkey::w_evp_pkey( std::shared_ptr<EVP_PKEY> fromPkey )
{
  _body = fromPkey;
}

bool w_evp_pkey::savePub( std::string path )
{
  BIO *write_bio_fp;
  write_bio_fp = BIO_new( BIO_s_file() );
  write_bio_fp = BIO_new_file( path.c_str() , "w" );

  PEM_write_bio_PUBKEY( write_bio_fp , _body.get() );

  BIO_vfree( write_bio_fp );
  return true;
}

bool w_evp_pkey::savePri( std::string path, std::string pass )
{
  BIO* write_bio_fp;
  write_bio_fp = BIO_new( BIO_s_file() );
  write_bio_fp = BIO_new_file( path.c_str() , "w" );

  if( pass.size() > 0 )
	PEM_write_bio_PKCS8PrivateKey( write_bio_fp , _body.get() , EVP_des_ede3_cbc(), pass.c_str() , pass.size() , NULL , NULL);
  else
	PEM_write_bio_PKCS8PrivateKey( write_bio_fp , _body.get() , EVP_des_ede3_cbc(), NULL, 0 , NULL , NULL);
	
  BIO_vfree( write_bio_fp );
  return true;
}



bool w_evp_pkey::loadPub( std::string path )
{
  BIO *read_bio_fp = NULL;
  read_bio_fp = BIO_new( BIO_s_file() );

  read_bio_fp = BIO_new_file( path.c_str() ,"r");
  if( read_bio_fp == nullptr ) return false;

  _body = std::shared_ptr<EVP_PKEY>( PEM_read_bio_PUBKEY( read_bio_fp , NULL, NULL, NULL ) , EVP_PKEY_deleter() );

  BIO_vfree( read_bio_fp );
  return true;
}
bool w_evp_pkey::loadPri( std::string path, std::string pass )
{
  BIO *read_bio_fp = NULL;
  read_bio_fp = BIO_new( BIO_s_file() );

  read_bio_fp = BIO_new_file( path.c_str() ,"r");
  if( read_bio_fp == nullptr ) return false;
  
  std::shared_ptr<w_evp_pkey> ret = std::make_shared<w_evp_pkey>();

  if( pass.size() > 0 )
	_body = std::shared_ptr<EVP_PKEY>( PEM_read_bio_PrivateKey( read_bio_fp , NULL, NULL, (char *)pass.c_str() ), EVP_PKEY_deleter() );
  else
	_body = std::shared_ptr<EVP_PKEY>( PEM_read_bio_PrivateKey( read_bio_fp , NULL, NULL, NULL ) , EVP_PKEY_deleter() );

  BIO_vfree( read_bio_fp );
  return true;

}

std::shared_ptr<EVP_PKEY> w_evp_pkey::pkey()
{
  return _body;
}

EVP_PKEY* w_evp_pkey::rawPkey()
{
  return this->pkey().get();
}


void w_evp_pkey::print() const
{
  if( _body == nullptr ){
	std::cout << "Key not set" << "\n";
	return;
  }

  BIO *output_bio = BIO_new_fp( stdout , 0 );
  EVP_PKEY_print_private( output_bio , _body.get() ,  0 , NULL ); std::cout << "\n\n";
  EVP_PKEY_print_public( output_bio, _body.get(),  0, NULL); std::cout << "\n";

  BIO_vfree( output_bio );
}


void w_evp_pkey::pkey( std::shared_ptr<EVP_PKEY> fromPkey )
{
  _body = fromPkey;
}


std::shared_ptr<EVP_PKEY> empty_pkey()
{
  EVP_PKEY *pkey = EVP_PKEY_new();
  return std::shared_ptr<EVP_PKEY>(pkey, EVP_PKEY_deleter() );
}


std::shared_ptr<EVP_PKEY> rsa_pkey( int keyBits, int engine )
{
  EVP_PKEY *pkey = nullptr;
  EVP_PKEY_CTX *pctx;

  pkey = EVP_PKEY_new();
  if( pkey == nullptr ) return nullptr;

  pctx = EVP_PKEY_CTX_new_id( EVP_PKEY_RSA, NULL );
  if( pctx == nullptr ) return nullptr;

  if( EVP_PKEY_keygen_init( pctx ) <= 0 ){
	EVP_PKEY_CTX_free(pctx);
	return nullptr;
  }

  if( EVP_PKEY_CTX_set_rsa_keygen_bits( pctx, keyBits ) <= 0 )
  {
	EVP_PKEY_CTX_free( pctx );
	return nullptr;
  }

  /*
  if( EVP_PKEY_CTX_set_default_rng_nid( pctx , engine ) )
  {
	EVP_PKEY_CTX_free( pctx );
	return nullptr;
  }
  */

  if( EVP_PKEY_keygen( pctx , &pkey ) <= 0 )
  {
	EVP_PKEY_CTX_free( pctx );
	return nullptr;
  }

  EVP_PKEY_CTX_free( pctx );
  return std::shared_ptr<EVP_PKEY>(pkey, EVP_PKEY_deleter() );
}


std::shared_ptr<EVP_PKEY> ecdsa_pkey( int engine )
{
  /*
  EVP_PKEY *pkey = nullptr;
  EVP_PKEY_CTX *pctx;

  pkey = EVP_PKEY_new();
  pctx = EVP_PKEY_CTX_new_id( EVP_PKEY_EC,  NULL );

  */
  return nullptr;
}


std::shared_ptr<w_evp_pkey> w_empty_pkey()
{
  std::shared_ptr<w_evp_pkey> ret = std::make_shared<w_evp_pkey>( empty_pkey() );
  // std::shared_ptr<EVP_PKEY> pkey = empty_pkey();
  // ret->pkey( pkey );

  return ret;
}

std::shared_ptr<w_evp_pkey> w_rsa_pkey( int keyBits )
{
  std::shared_ptr<w_evp_pkey> ret = std::make_shared<w_evp_pkey>( rsa_pkey(keyBits) );
  //std::shared_ptr<EVP_PKEY> pkey = rsa_pkey( keyBits );
  // ret->pkey( pkey );

  return ret;
}


};
