#ifndef AFC19124_7F4F_4268_8F0F_804853870D8B
#define AFC19124_7F4F_4268_8F0F_804853870D8B



#include <unistd.h>
#include <iostream>
#include <array>


#define CACHE_BLOCK_COUNT 10;


namespace miya_db{


class Mapper;




class CacheTable
{

private:

	Mapper *_mapper;

	std::array< void* , 10 > _cacheTable; // arraySizeはどう最適化するか

public:

	CacheTable( int fd );

	//void* map( /* ページアウトするキャッシュブロックインデックス*/ , /* ページインするオフセット*/ );
	//void sync( /* 同期するキャッシュブロックインデックス */);


	bool init();

	void* operator []( unsigned short idx ){
		return _cacheTable[idx];
	}

	unsigned short pageFault( unsigned short idx , unsigned int frame ); // pageOutとpageInを組み合わせる

	void pageOut( unsigned short idx );
	void pageIn( unsigned int frame ); 

};







}; // clsoe miya_db namespace

#endif // AFC19124_7F4F_4268_8F0F_804853870D8B

