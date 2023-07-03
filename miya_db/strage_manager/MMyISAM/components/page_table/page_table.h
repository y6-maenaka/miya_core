#ifndef CA8E8924_2243_4486_A648_8ED0EE3A95F0
#define CA8E8924_2243_4486_A648_8ED0EE3A95F0

#include <iostream>
#include <cmath>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>



namespace miya_db{


class CacheTable;


/* POINTER SIZE は 5 バイト
 	  - page_num(3) + offset(2)

*/



// 1 ノードの保存するデータ :  キー情報 + リンク情報 + データポインタ



#define ENTRY_CNT = pow(2, 8*3); //  [ bytes ]
#define ENTRY_SIZE = pow(2 , 8*2); // [ bytes ] 



struct Entry{

	unsigned char _entryBit = {0}; //  freme(6) ,    statusBit[ 存在ビット、修正ビット、参照ビット ]
													 //  000000(frame) , 0(存在ビット), 0(修正ビット), 0(参照ビット)
													 //  000000 0 0 0 0 
	
	unsigned short cachedIdx();

	bool invalidBit();

	bool referenceBit(); // getter
	void referencedBit( bool status ); // setter


	// bool exist(){ return (flags & 0x80) >> 7};
} __attribute__((__packed__));







/*
class PageTable{
	
private:
	const char *_filePath = nullptr;
	int _fd; // マッピングされるファイルディスクリプタ
	 
	CacheTable *_cacheManager; // オブジェクト名を変更する

	Entry	*_entryList; // exist(num), noexist(-1)

	struct referenceControl // LRUの近似
	{
		Entry *_invalidEntryList;
		unsigned short _referencePtr;

		void incrementPtr();
		unsigned short leastRecentlyUsedPage();
		void swapInvalidEntry( unsigned short oldEntryIdx , unsigned int newEntryIdx );

	} _referenceControl;


public:
	PageTable( const char* targetFilePath );
	~PageTable();

	// 初期化でキャッシュマネージャーが起動される
	bool init(); // 使い始めるにはinit必須
							 
	void* inquire( unsigned char* virtualAddr ); // localAddr -> 5 bit // 実際には
};
*/






};


#endif // CA8E8924_2243_4486_A648_8ED0EE3A95F0

