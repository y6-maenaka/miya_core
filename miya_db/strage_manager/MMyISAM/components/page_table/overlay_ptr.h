#ifndef FE16A7A3_448F_4FF8_A422_1C11E25ECFB6
#define FE16A7A3_448F_4FF8_A422_1C11E25ECFB6


#include <string.h>
#include <memory>
#include <cmath>
#include <string>



/*
 問題点

 ・addr() で取り出される値はマッピングされたポインタを直接取りに行くので暗黙的にPageFaultが発生する可能性がある
*/




namespace miya_db{

class CacheTable;


constexpr unsigned int OPTR_ADDR_LENGTH = 5;


// オーバレイ元のポインタは秘匿する
class optr
{
private:
	unsigned char _addr[5];
	//CacheTable *_cacheTable;
	std::shared_ptr<CacheTable> _cacheTable;

public:
	optr(){}; 
	optr( unsigned char *addr ); // フリーメモリ管理領域にはアクセスしないようにする
	// optr( unsigned char *addr , CacheTable *cacheTable );
	optr( unsigned char *addr , std::shared_ptr<CacheTable> cacheTable );

	//void cacheTable( CacheTable *cacheTable );
	void cacheTable( std::shared_ptr<CacheTable> cacheTable );
	// CacheTable *cacheTable(){ return _cacheTable; };
	std::shared_ptr<CacheTable> cacheTable(){ return _cacheTable; };

	unsigned char *addr();// getter // get with unsigned char[5]
	unsigned char *addr() const;
	std::shared_ptr<unsigned char> caddr() const;
	void addr( unsigned long ulongOptr );
	void addr( optr* from );
	void addr( unsigned char* from );

	unsigned short frame() const;
	unsigned short offset() const;

	unsigned char value(); // getter
	void value( unsigned char target ); // setter

	//OverlayPtr* operator []( size_t n ); 
	std::shared_ptr<optr> operator +( unsigned long addend ) const;
	//std::shared_ptr<optr> operator +( unsigned long addend ) const;

	std::unique_ptr<unsigned char> mapToMemory( unsigned int size );

	void printAddr() const;
	void printValueContinuously( unsigned int length ) const;

	bool operator ==( const optr& op ) const;
	bool operator !=( const optr& op ) const;
	struct Hash;
};


inline bool optr::operator ==( const optr& op ) const
{
	return ( memcmp( _addr , op.caddr().get(), sizeof(_addr) ) == 0 );
}

inline bool optr::operator !=( const optr& op ) const
{
	return !(this->operator==(op));
}

struct optr::Hash
{
	std::size_t operator()( const optr& op ) const;
};


inline std::size_t optr::Hash::operator()( const optr& op ) const
{
	std::string bytes;
	std::shared_ptr<unsigned char> caddr = op.caddr();
	std::copy( caddr.get() , caddr.get() + 5 , std::back_inserter(bytes) );
	return std::hash<std::string>()(bytes);
}



// ポインターとして利用できるように
// ポインタ一に対して値を挿入できるように





};


#endif // B4F18223_98AB_4CFC_ADEE_9F379C0A753E
