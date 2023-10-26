#ifndef A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
#define A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF



#include "./base_command.h"

namespace miya_chain
{



/*
 １つのSiriアライズしたブロックデータを送信する
 ※ ブロックに格納されている全トランザクションのRawデータを含む
*/



struct MiyaChainMSG_BLOCK : public MiyaChainPayloadFunction
{
public:
    static constexpr unsigned char command[12] = "block";

		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

};



}


#endif // A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF



