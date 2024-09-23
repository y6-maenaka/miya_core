#ifndef EC36BD3E_674E_4812_91F6_009697259017
#define EC36BD3E_674E_4812_91F6_009697259017


#include "./command_utils.hpp"


namespace chain
{
/*
    リモートノードのメモリープールにあるトランザクションTxIDを要求するメッセージ
    ※ 返答するのはTxID !!! 本データはgetdataを用いる
		リモートノードの全てのmempool内のトランザクションを要求する
*/

struct MiyaCoreMSG_MEMPOOL
{
private:
	unsigned char _blank[4] = {0x00, 0x00, 0x00, 0x00}; // mempoolメッセージは全てのtxIDを要求するためデータは空で良い

public:
    static constexpr char command[12] = "mempool";

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


}; // namespace chain


#endif // EC36BD3E_674E_4812_91F6_009697259017
