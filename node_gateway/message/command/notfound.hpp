#ifndef D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69
#define D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69


#include <memory>
#include "./inv.hpp"


namespace chain
{

/*
    見つからなかった場合
		getdataに対する応答
*/

struct MiyaCoreMSG_NOTFOUND
{
private:
  MiyaCoreMSG_INV _inv; // 中身はTypeIDのみ有効値を格納

public:
  using ref = std::shared_ptr<MiyaCoreMSG_NOTFOUND>;
  static constexpr char command[12] = "notfound";

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
  
  struct MiyaCoreMSG_INV &get_inv();
  std::vector<std::uint8_t> export_to_binary() const;
};


/* MiyaCoreMSG_INV& MiyaCoreMSG_NOTFOUND::get_inv()
{
  return _inv;
} */

size_t MiyaCoreMSG_NOTFOUND::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
    return _inv.exportRaw( retRaw );
}


}; // namespace chain


#endif
