#ifndef BE75CE29_95AE_41A8_A9D7_39C43A973FC0
#define BE75CE29_95AE_41A8_A9D7_39C43A973FC0

#include "./base_command.h"
#include "./inv/inv.h"


namespace miya_chain
{


/*
		[ 要求 ](複数)
    invメッセージで受信したオブジェクトのリストの中から特定のデータオブジェクトをリモートノードに要求する
    要求可能なデータはメモリープールまたはリレーセットにあるデータのみ
    ※ブロックに格納されたトランザクションデータにはアクセスできない
*/


struct MiyaChainMSG_GETDATA : public MiyaChainPayloadFunction
{
private:
	MiyaChainMSG_INV _inv;

public:
    static constexpr char command[12] = "getdata";

		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ){};
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ){};

    void inv( struct MiyaChainMSG_INV target );

};

}


#endif // BE75CE29_95AE_41A8_A9D7_39C43A973FC0
