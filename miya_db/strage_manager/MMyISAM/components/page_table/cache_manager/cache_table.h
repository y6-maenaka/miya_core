#ifndef AFC19124_7F4F_4268_8F0F_804853870D8B
#define AFC19124_7F4F_4268_8F0F_804853870D8B



#include <unistd.h>
#include <iostream>
#include <array>
#include <algorithm>

#include "../overlay_ptr.h"
#include "./cache_table_LRU.h"




namespace miya_db{

class Mapper;







class CacheTable
{

public:
//private:

	//Mapper *_mapper;
	std::shared_ptr<Mapper> _mapper;

	struct
	{
	public:
			short _cacheingList[ CACHE_BLOCK_COUNT ]; // キャッシュしているフレームのリスト
			void* _mappingList[ CACHE_BLOCK_COUNT ]; // 実際のポインタとオーバレイポインタの対応表

			LRU _LRU;
	} _cacheList;


	unsigned short pageFault( unsigned int frame ); // pageOutとpageInを組み合わせる																								
	int pageOut( unsigned short idx );
	int pageIn( unsigned short idx , unsigned int frame ); 

	// キャッシュしているフレームのインデックスを返却する
	short cacheFind( unsigned short frame );

public:
	void cacheingList();
	void invalidList();

	// Mapper *mapper(){ return _mapper; }; // getter
	std::shared_ptr<Mapper> mapper(){ return _mapper; };

	// bool init();
	CacheTable( int fd );

	// unsigned char* operator []( unsigned short frame );	
	void* convert( optr *src );
	void* convert( unsigned char* src );

	void syncForce( unsigned short targetMappedIndex );
};







}; // clsoe miya_db namespace

#endif // AFC19124_7F4F_4268_8F0F_804853870D8B

