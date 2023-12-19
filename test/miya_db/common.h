#ifndef A4A5DC0F_703E_4104_963C_E92656AAB85D
#define A4A5DC0F_703E_4104_963C_E92656AAB85D




#include <iostream>


#define DB_KEY_LENGTH 20

std::shared_ptr<unsigned char> generateDBKey( const char* from )
{
  std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[DB_KEY_LENGTH] );
  memcpy( ret.get() , from , DB_KEY_LENGTH );
  return ret;
}


std::shared_ptr<unsigned char> generateDBValue( const char* from , size_t fromLength )
{
  std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[fromLength] );
  memcpy( ret.get() , from , fromLength );
  return ret;
}


std::pair< std::shared_ptr<unsigned char> , std::shared_ptr<unsigned char> > generateDBKeyValuePair( const char* keyFrom , const char* valueFrom , size_t valueLength )
{
  std::shared_ptr<unsigned char> retKey = generateDBKey( keyFrom );
  std::shared_ptr<unsigned char> retValue = generateDBValue( valueFrom , valueLength );

  return std::make_pair( retKey , retValue );
}


int verifyDataConsistency( std::shared_ptr<unsigned char> origin, std::shared_ptr<unsigned char> response , size_t valueLength )
{
  return memcmp( origin.get() , response.get() , valueLength );
}


#endif //
