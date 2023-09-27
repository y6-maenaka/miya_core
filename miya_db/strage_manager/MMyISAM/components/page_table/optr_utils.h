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





optr *omemcpy( optr *dest , optr *src , unsigned long n );
optr *omemcpy( optr *dest , void *src , unsigned long n );
void *omemcpy( void* dest , optr *src , unsigned long n );



std::shared_ptr<optr> omemcpy( std::shared_ptr<optr> dest , std::shared_ptr<optr> src , unsigned long n );
std::shared_ptr<optr> omemcpy( std::shared_ptr<optr> dest , void *src , unsigned long n );
void *omemcpy( void* dest , std::shared_ptr<optr> src , unsigned long n );




/*
optr *omemcpy( optr* dest , optr* src , unsigned long n )
optr *omemcpy( optr *dest , void *src , unsigned long n );
void *omemcpy( void* dest , optr* src , unsigned long n );
*/



/* キャッシュされていない場所は0が返却される */

int ocmp( optr* o1 , optr* o2, unsigned long size );
int ocmp( optr* o1 , void* p2, unsigned long size );
int ocmp( void* p1, optr* o2 , unsigned long size );




}; // close miya_db namespace


#endif // FDDF8152_AB99_4673_A5E8_9F98C130F129



