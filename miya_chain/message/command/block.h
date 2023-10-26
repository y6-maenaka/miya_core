#ifndef A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
#define A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF



#include "./base_command.h"

namespace miya_chain
{



/*
 １つのSiriアライズしたブロックデータを送信する
 ※ ブロックに格納されている全トランザクションのRawデータを含む
 つまり,指定したブロックデータを丸ごとバイナリで送信する
*/



struct MiyaChainMSG_BLOCK : public MiyaChainPayloadFunction
{
private:
	// ※生のブロックデータを送信する
		

public:
    static constexpr unsigned char command[12] = "block";

		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

};



}


#endif // A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF



