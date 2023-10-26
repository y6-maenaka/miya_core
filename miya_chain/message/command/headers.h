#ifndef C7FDBB05_3027_4D94_B5F4_4B17B9A11D23
#define C7FDBB05_3027_4D94_B5F4_4B17B9A11D23



#include "./base_command.h"


namespace miya_chain
{




/*
    getheadersメッセージを受信したノードが,1つ以上のブロックヘッダを返答する際に使用する
*/


struct MiyaChainMSG_HEADERS
{
public:
    static constexpr unsigned char command[12] = "headers";


	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


};


}


#endif // C7FDBB05_3027_4D94_B5F4_4B17B9A11D23

