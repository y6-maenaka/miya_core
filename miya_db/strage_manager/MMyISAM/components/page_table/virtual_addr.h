#ifndef EDE5692A_1133_4138_9D45_89CC0A01ED81
#define EDE5692A_1133_4138_9D45_89CC0A01ED81


#include <iostream>





namespace miya_db{


class PageTable;



struct VirtualAddr
{

private:
	unsigned char _virtualAddr; // address body
	void* _addrCache = nullptr;

	PageTable *_pageTable;

public:
	unsigned char *frame();
	unsigned char *offset();

	void* toPtr( bool isCacheInvalidation = false ); // 明示的なポインタ変換
	//void *operator ^ () const ; // ポインタ変換 // 読み取り専用
};





}; // close miya_db namespace


#endif // EDE5692A_1133_4138_9D45_89CC0A01ED81

