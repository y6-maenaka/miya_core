#ifndef FDDF8152_AB99_4673_A5E8_9F98C130F129
#define FDDF8152_AB99_4673_A5E8_9F98C130F129

#include <iostream>
#include <cstddef>
#include "./overlay_ptr.h"




namespace miya_db
{



optr *omemcpy( optr *dest , optr *src , unsigned long n );

optr *omemcpy( optr *dest , void *src , unsigned long n );

void *omemcpy( void* dest , optr *src , unsigned long n );


/* キャッシュされていない場所は0が返却される */
int ocmp( void* p1, optr* o2 , unsigned long size );
int ocmp( optr* o1 , void* p2, unsigned long size );
int ocmp( optr* o1 , optr* o2, unsigned long size );




}; // close miya_db namespace


#endif // FDDF8152_AB99_4673_A5E8_9F98C130F129



