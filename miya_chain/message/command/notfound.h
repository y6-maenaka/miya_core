#ifndef D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69
#define D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69


#include "./base_command.h"
#include "./inv/inv.h"


namespace miya_chain
{

/*
    見つからなかった場合
		getdataに対する応答
*/



struct MiyaChainMSG_NOTFOUND
{
private:
		MiyaChainMSG_INV _inv; // 中身は空だが,公式リファレンスでは宣言してあった

public:
    static constexpr char command[12] = "notfound";


		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


};

}





#endif 

