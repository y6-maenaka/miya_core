#ifndef D562E07E_6D05_4548_8CF9_EBE0406A6ADA
#define D562E07E_6D05_4548_8CF9_EBE0406A6ADA



#include <vector>




namespace tx
{
	class P2PKH;
};



namespace miya_chain
{




struct TxInvMessage // inventory vector
{	
	std::vector<tx::P2PKH> _txVector;
};





};


#endif // 

