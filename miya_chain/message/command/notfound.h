#ifndef D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69
#define D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69


#include "./base_command.h"


namespace miya_chain
{

/*
    見つからなかった場合
*/



struct MiyaChainMSG_NOTFOUND
{
public:
    static constexpr unsigned char command[12] = "notfound";


		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


};

}





#endif 

