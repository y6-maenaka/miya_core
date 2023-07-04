#ifndef FEE61693_0555_46FA_9A34_80AFCCBEB683
#define FEE61693_0555_46FA_9A34_80AFCCBEB683


#include <algorithm>

constexpr unsigned short CACHE_BLOCK_COUNT = 3;


namespace miya_db{



struct LRU
{
public:
	bool _invalidCacheList[ CACHE_BLOCK_COUNT ]; // 参照ビット
	unsigned short _referencePtr = 0; // 参照ポインタ
	// bool _invalidCacheList = {true};

	void incrementReferencePtr();
	void reference( unsigned short idx );
	unsigned short targetOutPage();

	LRU();
};



};

#endif // FEE61693_0555_46FA_9A34_80AFCCBEB683



