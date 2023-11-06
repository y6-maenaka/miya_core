#ifndef E81917A3_924A_4690_938C_E7F1FB47E25E
#define E81917A3_924A_4690_938C_E7F1FB47E25E



#include "../../components/index_manager/btree.h"




namespace miya_db
{


struct ONodeConversionTable;



class SafeONodeItemSet : public ONodeItemSet
{
private:
	std::shared_ptr<ONodeConversionTable> &_conversionTable;

public:
	child();
};




class SafeONode : public ONode
{

private:
	std::shared_ptr<ONodeConversionTable> &_conversionTable;


public:

	parent()
		child()
};










};



#endif // E81917A3_924A_4690_938C_E7F1FB47E25E



