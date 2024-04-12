#ifndef AB364825_7E4D_4DE1_94FF_AE85D762CBF9
#define AB364825_7E4D_4DE1_94FF_AE85D762CBF9


#include <memory>
#include <iostream>
#include <algorithm>
#include <unistd.h>


#include "../../../share/json.hpp"
#include "../query_context/query_context.h"


namespace miya_db{

class UnifiedStrageManager;
struct QueryContext;





/* -----------------------------------------

	[ Query Format ]

	< ADD(Request) >
	 - id : .....
	 - content
		|- query : 1
		|- key : xxxxxxxxxxxxxxxxxxxx
		|- value : *********************


	< ADD(Response) >
	 - id : ....
	 - status : true/false	
	 - timestamp : timestamp






	< GET(Request) >
	 - id : ....
	 - query : 2 
	 - key : xxxxxxxxxxxxxxxxxxxx


	< GET(Response) > 
		- id : .....
		-content
			|- status : true/false
			|- value : **************
			|- timestamp : timestamp

-------------------------------------------- */















class QueryParser
{
private:

public:
	//int *parseQuery( unsigned char* query , unsigned int querySize ); // return 成否フラグ
	UnifiedStrageManager *suggestTable();


	//void parseQuery( nlohmann::json &rawQuery );
	std::shared_ptr<QueryContext> parseQuery( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

	void hello();
};




}; // close miya_db namespace

#endif // AB364825_7E4D_4DE1_94FF_AE85D762CBF9

