#include "prev_out.h"


namespace tx
{



PrevOut::PrevOut()
{
	//_body._txID = std::make_shared<unsigned char>(256/8);
	_body._txID = std::shared_ptr<unsigned char>( new unsigned char[32] );
	memset( _body._txID.get(), 0x00 , 32 );
	memset( &(_body._index) , 0xff , sizeof(_body._index) );
}






std::shared_ptr<unsigned char> PrevOut::txID()
{
	return _body._txID;
}

void PrevOut::txID( std::shared_ptr<unsigned char> target )
{
	if( target == nullptr )
	{
		memset( _body._txID.get() , 0x00 , 32 );
		return;
	}

	_body._txID = target;
}


void PrevOut::txID( const unsigned char *target )
{
	if( target == nullptr )
	{
		memset( _body._txID.get() , 0x00 , 32 );
		return;
	}
	std::copy( target , target + 32 , _body._txID.get() );
}



unsigned short PrevOut::index()
{
	#ifdef __linux__
		return static_cast<unsigned short>(be32toh(_body._index));
	#endif
	#ifdef __APPLE__
		return static_cast<unsigned short>(ntohl(_body._index));
	#endif
}

void PrevOut::index( int target )
{
	#ifdef __linux__
		_body._index = htobe32(target);
	#endif
	#ifdef __APPLE__
		_body._index = htonl(target);
	#endif
}


void PrevOut::index( uint32_t target )
{
	#ifdef __linux__
		_body._index = htobe32(target);
	#endif
	#ifdef __APPLE__
		_body._index = htonl(target);
	#endif
}



unsigned int PrevOut::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[ 32 + sizeof(_body._index)] ); // あとで修正する
	memcpy( (*retRaw).get() , _body._txID.get() , 32 );
	memcpy( (*retRaw).get() + 32, &(_body._index) , sizeof(_body._index) );

	return 32 + sizeof(_body._index);
}




/*
int PrevOut::importRaw( std::shared_ptr<unsigned char> fromRaw )
{
	if( fromRaw == nullptr ) return -1;

	memcpy( _body._txID.get() , fromRaw.get() , 256/8 ); // あとでマジックナンバーは修正する
	memcpy( &(_body._index) , fromRaw.get() + (256/8) , sizeof(_body._index) );

	return ( (256/8) + sizeof(_body._index) ); // 流石に雑すぎる?
}
*/





int PrevOut::importRaw( unsigned char* fromRaw )
{
	if( _body._txID == nullptr ) return -1;
	// _body._txID = std::make_shared<unsigned char>(*fromRaw);


	memcpy( _body._txID.get(), fromRaw  , 256/8 );
	memcpy( &_body._index, fromRaw + (256/8) , sizeof(_body._index) );

	return ( (256/8) + sizeof(_body._index) ); // 流石に雑すぎる?
}


void PrevOut::print()
{
	std::cout << "TxID :: ";
	for( int i = 0; i<32; i++ ){
		printf("%02X", _body._txID.get()[i]);
	} std::cout << "\n";

	std::cout << "Index :: ";
	unsigned char uIndex[sizeof(_body._index)];
	memcpy( uIndex, &(_body._index) , sizeof(_body._index) );
	for( int i=0; i<sizeof(_body._index); i++ ){
		printf("%02X", uIndex[i] );
	} std::cout << "\n";
}


}; // close prev_out namespace
