#ifndef AFC19124_7F4F_4268_8F0F_804853870D8B
#define AFC19124_7F4F_4268_8F0F_804853870D8B



#include <unistd.h>
#include <iostream>
#include <array>


constexpr unsigned short CACHE_BLOCK_COUNT = 10;


namespace miya_db{

class Mapper;








class CacheTable
{

private:

	Mapper *_mapper;

	struct CacheList
	{
		private:
			unsigned short _cacheList[ CACHE_BLOCK_COUNT ] = {-1};
			unsigned char* _mappingList[ CACHE_BLOCK_COUNT ] = {nullptr};

		public:
			short cacheFind( unsigned short frame );

	} _cacheList;


	struct LRU
	{
		private:
			bool _invalidCacheList[ CACHE_BLOCK_COUNT ] = {true}; // LRU
			unsigned short _referencePtr = 0;
			// bool _invalidCacheList = {true};
		public:
			void operator++();
			void reference( unsigned short idx );
			unsigned short outPage();

	} _LRU;

	unsigned short pageFault( unsigned int frame ); // pageOutとpageInを組み合わせる																								
	void pageOut( unsigned short idx );
	void pageIn( unsigned short idx , unsigned int frame ); 

	bool init();

	unsigned short cacheFind( unsigned short frame );

public:

	CacheTable( int fd );



	unsigned char* operator []( unsigned short frame );
	




	// どのフレームがキャッシュされているか
	// LRUの実装
};







}; // clsoe miya_db namespace

#endif // AFC19124_7F4F_4268_8F0F_804853870D8B

