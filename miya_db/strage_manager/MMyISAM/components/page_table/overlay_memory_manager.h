#ifndef B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40
#define B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


#include "overlay_ptr.h"


namespace miya_db{


class CacheManager;


class OverlayMemoryManager
{
private:
	CacheManager *_cacheManager;

public:
	optr *generateOptr();
};


}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40



