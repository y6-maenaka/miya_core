#ifndef FDDF8152_AB99_4673_A5E8_9F98C130F129
#define FDDF8152_AB99_4673_A5E8_9F98C130F129

#include <iostream>
#include <cstddef>
#include "./overlay_ptr.h"




namespace miya_db
{





/*
template <class T>
T &omemcpy( T &dest , T &src , unsigned long n )
{	 
	for( unsigned long i = 0; i<n; i++ )
		(dest + i)->value( (*src + i)->value() );

	return dest;
};

template <class T>
T &omemcpy( T &dest , void *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++)
		(dest + i)->value( static_cast<unsigned char*>(src)[i] );	

	return dest;
};


template <class T>
void *omemcpy( void* dest , T &src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++ )	
		(static_cast<unsigned char*>(dest))[i] = (src +i)->value();

	return dest;
};
*/





optr *omemcpy( optr *dest , optr *src , unsigned long n )
{	 
	for( unsigned long i = 0; i<n; i++ )
		( *dest + i)->value( (*src + i)->value() );

	return dest;
};




optr *omemcpy( optr *dest , void *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++)
		(*dest + i)->value( static_cast<unsigned char*>(src)[i] );	

	return dest;
};



template <class T>
void *omemcpy( void* dest , optr *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++ )	
		(static_cast<unsigned char*>(dest))[i] = (*src +i)->value();

	return dest;
};



/*
optr *omemcpy( optr* dest , optr* src , unsigned long n )
optr *omemcpy( optr *dest , void *src , unsigned long n );
void *omemcpy( void* dest , optr* src , unsigned long n );
*/



/* キャッシュされていない場所は0が返却される */

template <class T>
int ocmp( T* o1 , T* o2, unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if( (((*o1)+i)->value()) !=  (((*o2)+i)->value()) )   break;
	}

	if( i == size ) return 0;
	return ( ((*o1)+i)->value() <=  ((*o2)+i)->value() ) ? -1 : 1;	
}





template <class T>
int ocmp( T* o1 , void* p2, unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if( (((*o1)+i)->value()) != ((static_cast<unsigned char*>(p2))[i]) )   break;
	}

	if( i == size ) return 0;
	return ( ((*o1)+i)->value() <= ((static_cast<unsigned char*>(p2))[i]) ) ? -1 : 1;
	
}




template <class T>
int ocmp( void* p1, T* o2 , unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if(  (static_cast<unsigned char*>(p1))[i] != ((*o2)+i)->value() ) break;
	}

	if( i == size ) return 0;
	return ( (static_cast<unsigned char*>(p1))[i] <= ((*o2)+i)->value()  ) ? -1 : 1;
	//return (( *(static_cast<unsigned char*>(p1))[i]) >= ((*o2)+i)->value() ) ? 1 : -1;	
}





}; // close miya_db namespace


#endif // FDDF8152_AB99_4673_A5E8_9F98C130F129



