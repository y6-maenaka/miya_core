#include "optr_utils.h"




namespace miya_db
{






optr *omemcpy( optr *dest , optr *src , unsigned long n )
{	 
	for( unsigned long i = 0; i<n; i++ )
		((*dest) + i)->value( ((*src) + i)->value() );

	return dest;
};





optr *omemcpy( optr *dest , void *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++)
		((*dest) + i)->value( static_cast<unsigned char*>(src)[i] );	

	return dest;
};




void *omemcpy( void* dest , optr *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++ )	
		(static_cast<unsigned char*>(dest))[i] = ((*src) +i)->value();

	return dest;
};






};


