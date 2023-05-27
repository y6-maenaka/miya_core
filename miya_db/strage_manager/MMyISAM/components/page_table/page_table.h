#ifndef CA8E8924_2243_4486_A648_8ED0EE3A95F0
#define CA8E8924_2243_4486_A648_8ED0EE3A95F0

#include <iostream>
#include <cmath>



namespace miya_db{


/* POINTER SIZE は 5 バイト
 	  - page_num(3) + offset(2)

*/



// 1 ノードの保存するデータ :  キー情報 + リンク情報 + データポインタ



constexpr unsigned int ENTRY_CNT = pow(2, 8*3);



struct Entry{

	unsigned char _entryBit = {0}; //  freme(6) ,    statusBit[ 存在ビット、修正ビット、参照ビット ]
													 //  000000(frame) , 0(存在ビット), 0(修正ビット), 0(参照ビット)
													 //  000000 0 0 0 0 
	
	bool invalidBit();

	bool referenceBit(); // getter
	void referencedBit( bool bit ); // setter


	// bool exist(){ return (flags & 0x80) >> 7};
} __attribute__((__packed__));








class PageTable{
	
private:

	const char *_filePath = nullptr;
	int _fd;
	
	CacheManager *_cacheManager;

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

	void init(); // 使い始めるにはinit必須
							 
	void* inquire( unsigned char* logicAddr ); // localAddr -> 5 bit // 実際には

};





};


#endif // CA8E8924_2243_4486_A648_8ED0EE3A95F0

