#ifndef F6ADB1E3_3173_4FE7_9769_06E79F856756
#define F6ADB1E3_3173_4FE7_9769_06E79F856756


#include "./overlay_ptr.h"


namespace miya_db
{

class CacheTable;



class OverlayPtrResolver
{

private:
	CacheTable *_cacheTable;

public:
	void* resolve( optr *src ); // 対応するポインタが得られる
	

};


}; // close miya_db namespace









#endif // F6ADB1E3_3173_4FE7_9769_06E79F856756



