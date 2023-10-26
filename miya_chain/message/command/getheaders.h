#ifndef B3C13BE4_E4A4_496E_971D_400E0D09945A
#define B3C13BE4_E4A4_496E_971D_400E0D09945A


#include "./base_command.h"


namespace miya_chain
{


/*
  ブロックのある地点からのブロックヘッダを提供するheadersメッセージを要求するメッセージ
  getblocksのより要求サイズが大きいver. ほとんど同じと思って良い 内部データも同じ 一旦使わなくていいかな
*/


struct MiyaChainMSG_GETHEADERS
{
  static constexpr unsigned char command[12] = "getheaders";

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


};


}


#endif // B3C13BE4_E4A4_496E_971D_400E0D09945A



