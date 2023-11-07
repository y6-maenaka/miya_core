#ifndef BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A
#define BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A



#include <iostream>
#include <memory>


#include "../../components/index_manager/index_manager.h"


namespace miya_db
{





class SafeOBtree;



class SafeIndexManager : public IndexManager
{

	std::shared_ptr<SafeOBtree> _masterBtree;
	

};






};


#endif //  BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A



