#include "page_table.h"


namespace miya_db{




unsigned short Entry::frame(){
	return static_cast<unsigned short>( _entryBit >> 4 );

}




bool Entry::invalidBit(){
	return static_cast<int>((_entryBit & 0x08) >> 3);
}



bool Entry::referenceBit() // getter
{
	return static_cast<int>((_entryBit & 0x01));
}



void Entry::referenceBit( bool status )
{
	
	if( status ){ // ビットを1にしたい場合
		_entryBit |= ( 1  ) 
	}
	else // ビットを0にしたい場合
	{
		_entryBit &= ~( 1 );
	}

	return;
}




/* ------------------------------------------------------ */
/* ==================== PAGE TABLE ====================== */
/* ------------------------------------------------------ */




PageTable::PageTable( const char *filePath )
{
	_filePath = filePath;

	_entryList = new Entry[ pow( 2, 8*2 ) ]; // ページテーブルのセットアップ
}



PageTable::~PageTable()
{
	// 全てのキャッシュテーブルのアンマップ

	delete _entryList;
}



bool PageTable::init()
{
	if( (_fd = open( filePath , O_RDWR )) < 0 ) return false;

	_cacheManager = new CacheManager( _fd );

	return true;
}



void PageTable::pageIn() // 間接的にmapperの操作を行う為パラメータの指定はダイレクトに
{

	_cacheTable


	return;
}


void PageTable::pageOut()
{
	return;
}





void* PageTable::inquire( unsigned char *logicAddr ){

	unsigned char frame[3]; unsigned int Uframe = 0;
	unsigned char offset[2];  unsigned short Uoffset = 0;

	memcpy( &frame , logicAddr, sizeof(frame) );
	memcpy( &offset , logicAddr + sizeof(pageIdx) , sizeof(offset) );

	Uframe = frame[0]; Uframe <<= 8;
	Uframe = frame[1]; Uframe <<= 8;
	Uframe = frame[2]; 

	Uoffset = offset[0]; Uoffset <<= 8;
	Uoffset = offset[1]; 

	Entry *entry;
	entry = _entryList[ Uframe ];


	if( !(entry->invalidBit()) ){ // pageFault

		unsigned short outIdx = _referenceControl->leastRecentlyUsedPage();

		return _cacheManager->pageFault( outIdx , Uframe );
	}

	else{ 
		_refelenceControl->incrementPtr();
		return _cacheTable[ index ];
	}

	/* この後,Converterにかける？ */

	return
};


