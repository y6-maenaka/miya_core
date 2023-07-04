#ifndef FE16A7A3_448F_4FF8_A422_1C11E25ECFB6
#define FE16A7A3_448F_4FF8_A422_1C11E25ECFB6


#include <string.h>




namespace miya_db{

class CacheTable;



// オーバレイ元のポインタは秘匿する
typedef class OverlayPtr
{
private:
	// unsigned char _optr;
	unsigned char _optr[5];
	CacheTable *_cacheTable;
	//OverlayPtrResolver *_optResolver;

public:
	OverlayPtr(){};
	OverlayPtr( unsigned char *optr );

	void cacheTable( CacheTable *cacheTable );

	unsigned char *optr(); // getter

	unsigned short frame();
	unsigned short offset();

	unsigned char value(); // getter
	void value( unsigned char target ); // setter

	//OverlayPtr* operator []( size_t n ); 
	OverlayPtr* operator =( OverlayPtr *_from );
	
	// void operator =( unsigned char target );

	// void operator =(void *ptr);

} optr;








// ポインターとして利用できるように
// ポインタ一に対して値を挿入できるように





};


#endif // B4F18223_98AB_4CFC_ADEE_9F379C0A753E
