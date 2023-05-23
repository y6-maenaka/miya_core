#ifndef CD595D1E_AA1A_439E_9D55_12032E706D4D
#define CD595D1E_AA1A_439E_9D55_12032E706D4D



#include "../unified_strage_engine/unified_strage_engine.h"


namespace miya_db{




struct Table
{
	char* _pathToDir;
	
	char *indexFilePath();
	char *dataFilePath();
}




class MMyISAM : public UnifiedStrageEngine { // 継承したほうがいい?

private:	

public:

	void update();
	void get();

};

}; // close miya_db namespace



#endif // CD595D1E_AA1A_439E_9D55_12032E706D4D
