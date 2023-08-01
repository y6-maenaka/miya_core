#ifndef FE16A7A3_448F_4FF8_A422_1C11E25ECFB6
#define FE16A7A3_448F_4FF8_A422_1C11E25ECFB6


#include <string.h>
#include <memory>




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
	OverlayPtr( unsigned char *optr ); // フリーメモリ管理領域にはアクセスしないようにする

	void cacheTable( CacheTable *cacheTable );

	unsigned char *optr(); // getter with unsigned char[5]
	void optr( unsigned long ulongOptr );

	unsigned short frame();
	unsigned short offset();

	unsigned char value(); // getter
	void value( unsigned char target ); // setter

	//OverlayPtr* operator []( size_t n ); 
	std::unique_ptr<OverlayPtr> operator +( unsigned long addend );


} optr;








// ポインターとして利用できるように
// ポインタ一に対して値を挿入できるように





};


#endif // B4F18223_98AB_4CFC_ADEE_9F379C0A753E
