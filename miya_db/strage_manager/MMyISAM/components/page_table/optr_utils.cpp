#include "optr_utils.h"




namespace miya_db
{



optr *omemcpy( optr *dest , optr *src , unsigned long n )
{	 
	for( unsigned long i = 0; i<n; i++ )
		( *dest + i)->value( (*src + i)->value() );

	return dest;
};




optr *omemcpy( optr *dest , void *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++)
	{
		//printf(" [%02X]\n", (*dest + i)->value() );
		(*dest + i)->value( static_cast<unsigned char*>(src)[i] );	
	}

	return dest;
};

void *omemcpy( void* dest , optr *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++ )	
		(static_cast<unsigned char*>(dest))[i] = (*src +i)->value();

	return dest;
}







std::shared_ptr<optr> omemcpy( std::shared_ptr<optr> dest , std::shared_ptr<optr> src , unsigned long n )
{	 
	for( unsigned long i = 0; i<n; i++ )
		( *(dest.get()) + i)->value( (*(src.get()) + i)->value() );

	return dest;
};




std::shared_ptr<optr> omemcpy( std::shared_ptr<optr> dest , void *src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++)
	{
		//printf(" [%02X]\n", (*dest + i)->value() );
		(*(dest.get()) + i)->value( static_cast<unsigned char*>(src)[i] );	
	}

	return dest;
};


void *omemcpy( void* dest , std::shared_ptr<optr> src , unsigned long n )
{
	for( unsigned long i = 0; i<n; i++ )	
		(static_cast<unsigned char*>(dest))[i] = (*(src.get()) +i)->value();

	return dest;
}













/* キャッシュされていない場所は0が返却される */

int ocmp( optr* o1 , optr* o2, unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if( (((*o1)+i)->value()) !=  (((*o2)+i)->value()) )   break;
	}

	if( i == size ) return 0;
	return ( ((*o1)+i)->value() <=  ((*o2)+i)->value() ) ? -1 : 1;	
}





int ocmp( optr* o1 , void* p2, unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if( (((*o1)+i)->value()) != ((static_cast<unsigned char*>(p2))[i]) )   break;
	}

	if( i == size ) return 0;
	return ( ((*o1)+i)->value() <= ((static_cast<unsigned char*>(p2))[i]) ) ? -1 : 1;
	
}




int ocmp( void* p1, optr* o2 , unsigned long size )
{
	int i=0;
	for( i=0; i<size; i++ ){
		if(  (static_cast<unsigned char*>(p1))[i] != ((*o2)+i)->value() ) break;
	}

	if( i == size ) return 0;
	return ( (static_cast<unsigned char*>(p1))[i] <= ((*o2)+i)->value()  ) ? -1 : 1;
	//return (( *(static_cast<unsigned char*>(p1))[i]) >= ((*o2)+i)->value() ) ? 1 : -1;	
}






};


