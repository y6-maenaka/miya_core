#ifndef FE16A7A3_448F_4FF8_A422_1C11E25ECFB6
#define FE16A7A3_448F_4FF8_A422_1C11E25ECFB6


#include <string.h>
#include <memory>



namespace miya_db{

class CacheTable;


constexpr unsigned int OPTR_ADDR_LENGTH = 5;


// オーバレイ元のポインタは秘匿する
class optr
{
private:

	unsigned char _addr[5];
	CacheTable *_cacheTable;

public:
	optr(){}; 
	optr( unsigned char *optr ); // フリーメモリ管理領域にはアクセスしないようにする

	void cacheTable( CacheTable *cacheTable );

	unsigned char *addr(); // getter // get with unsigned char[5]
	void addr( unsigned long ulongOptr );

	unsigned short frame();
	unsigned short offset();

	unsigned char value(); // getter
	void value( unsigned char target ); // setter

	//OverlayPtr* operator []( size_t n ); 
	std::unique_ptr<optr> operator +( unsigned long addend );

	std::unique_ptr<unsigned char> mapToMemory( unsigned int size );

};








// ポインターとして利用できるように
// ポインタ一に対して値を挿入できるように





};


#endif // B4F18223_98AB_4CFC_ADEE_9F379C0A753E
