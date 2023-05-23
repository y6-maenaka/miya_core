#ifndef AB364825_7E4D_4DE1_94FF_AE85D762CBF9
#define AB364825_7E4D_4DE1_94FF_AE85D762CBF9




namespace miya_db{

class UnifiedStrageManager;



class QueryParser
{
private:
	struct QueryContext
	{	
		int command;
		
	} _queryContext;

public:
	int *parseQuery( unsigned char* query , unsigned int querySize ); // return 成否フラグ
	UnifiedStrageManager *suggestTable();


};




}; // close miya_db namespace

#endif // AB364825_7E4D_4DE1_94FF_AE85D762CBF9

