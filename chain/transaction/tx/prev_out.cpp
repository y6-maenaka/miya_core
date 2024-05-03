#include "prev_out.h"


namespace tx
{


prev_out::prev_out()
{
	//_meta._txID = std::make_shared<unsigned char>(256/8);
	_meta._txID = std::shared_ptr<unsigned char>( new unsigned char[32] );
	memset( _meta._txID.get(), 0x00 , 32 );
	memset( &(_meta._index) , 0xff , sizeof(_meta._index) );
}


std::shared_ptr<unsigned char> prev_out::txID()
{
	return _meta._txID;
}

void prev_out::txID( std::shared_ptr<unsigned char> target )
{
	if( target == nullptr )
	{
		memset( _meta._txID.get() , 0x00 , 32 );
		return;
	}

	_meta._txID = target;
}

void prev_out::txID( const unsigned char *target )
{
	if( target == nullptr )
	{
		memset( _meta._txID.get() , 0x00 , 32 );
		return;
	}
	std::copy( target , target + 32 , _meta._txID.get() );
}

unsigned short prev_out::index()
{
	#ifdef __linux__
		return static_cast<unsigned short>(be32toh(_meta._index));
	#endif
	#ifdef __APPLE__
		return static_cast<unsigned short>(ntohl(_meta._index));
	#endif
}

void prev_out::index( int target )
{
	#ifdef __linux__
		_meta._index = htobe32(target);
	#endif
	#ifdef __APPLE__
		_meta._index = htonl(target);
	#endif
}

void prev_out::index( uint32_t target )
{
	#ifdef __linux__
		_meta._index = htobe32(target);
	#endif
	#ifdef __APPLE__
		_meta._index = htonl(target);
	#endif
}

unsigned int prev_out::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[ 32 + sizeof(_meta._index)] ); // あとで修正する
	memcpy( (*retRaw).get() , _meta._txID.get() , 32 );
	memcpy( (*retRaw).get() + 32, &(_meta._index) , sizeof(_meta._index) );

	return 32 + sizeof(_meta._index);
}


/*
int prev_out::importRaw( std::shared_ptr<unsigned char> fromRaw )
{
	if( fromRaw == nullptr ) return -1;

	memcpy( _meta._txID.get() , fromRaw.get() , 256/8 ); // あとでマジックナンバーは修正する
	memcpy( &(_meta._index) , fromRaw.get() + (256/8) , sizeof(_meta._index) );

	return ( (256/8) + sizeof(_meta._index) ); // 流石に雑すぎる?
}
*/


int prev_out::importRaw( unsigned char* fromRaw )
{
	if( _meta._txID == nullptr ) return -1;
	// _meta._txID = std::make_shared<unsigned char>(*fromRaw);

	memcpy( _meta._txID.get(), fromRaw  , 256/8 );
	memcpy( &_meta._index, fromRaw + (256/8) , sizeof(_meta._index) );

	return ( (256/8) + sizeof(_meta._index) ); // 流石に雑すぎる?
}

void prev_out::print()
{
	std::cout << "TxID :: ";
	for( int i = 0; i<32; i++ ){
		printf("%02X", _meta._txID.get()[i]);
	} std::cout << "\n";

	std::cout << "Index :: ";
	unsigned char uIndex[sizeof(_meta._index)];
	memcpy( uIndex, &(_meta._index) , sizeof(_meta._index) );
	for( int i=0; i<sizeof(_meta._index); i++ ){
		printf("%02X", uIndex[i] );
	} std::cout << "\n";
}

std::vector<std::uint8_t> prev_out::export_to_binary() const
{
  std::array< std::uint8_t, sizeof(_meta) > ret_a;
  std::memcpy( ret_a.data(), &_meta, sizeof(_meta) );

  return std::vector<std::uint8_t>( ret_a.begin(), ret_a.end() );
}


}; // close prev_out namespace
