#ifndef B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40
#define B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40


namespace miya_db{


class CacheManager;
class optr;


class OverlayMemoryManager
{
private:
	CacheManager *_cacheManager;

public:
	optr *lastOptr();
};


}; // close miya_db namespace


#endif // B2BCCAA9_4237_45CF_8F1A_B7E2E523CD40



