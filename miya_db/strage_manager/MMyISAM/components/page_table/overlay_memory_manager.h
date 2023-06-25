#ifndef BA85B734_1B31_49C4_96CD_C1ACC7293375
#define BA85B734_1B31_49C4_96CD_C1ACC7293375



namespace miya_db
{

class CacheTable;
class optr;






class OverlayMemoryManager // OMM
{
private:
	CacheTable *_cacheTable;


public:
	void* convert( optr *src );

};





};


#endif // BA85B734_1B31_49C4_96CD_C1ACC7293375

