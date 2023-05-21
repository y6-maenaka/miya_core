#ifndef CA8E8924_2243_4486_A648_8ED0EE3A95F0
#define CA8E8924_2243_4486_A648_8ED0EE3A95F0

#include <iostream>



namespace miya_db{


/* POINTER SIZE は 5 バイト
 	  - page_num(3) + offset(2)

*/



// 1 ノードの保存するデータ :  キー情報 + リンク情報 + データポインタ



constexpr unsigned int ENTRY_CNT = pow(2, 8*3);


struct Entry{

	unsigned char statusBit; // 存在ビット(1)、修正ビット(1)、参照ビット(1)、予約ビット(5)
	unsigned char pageIdx[3]; // pageIdx 

	Entry() : flags(0) {};

	// bool exist(){ return (flags & 0x80) >> 7};
};






class PageTable{
	
private:
	//Entry entrys[ pow(2,3*8) ];			
	Entry *_entrys;

public:
	PageTable();
};





};


#endif // CA8E8924_2243_4486_A648_8ED0EE3A95F0

