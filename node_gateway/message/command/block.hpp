#ifndef A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
#define A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF


// #include <chain/block/block.h>
#include <memory>
#include <chain/block/block.h>
#include <chain/block/block.hpp>


namespace chain
{


struct Block;

/*
 １つのSiriアライズしたブロックデータを送信する
 ※ ブロックに格納されている全トランザクションのRawデータを含む
 つまり,指定したブロックデータを丸ごとバイナリで送信する
*/

struct MiyaCoreMSG_BLOCK
{
private:
  // ※生のブロックデータを送信する
  block::ref _block;

public:
  using ref = std::shared_ptr<struct MiyaCoreMSG_BLOCK>;
  static constexpr char command[12] = "block";

  block::ref get_block();

  std::vector<std::uint8_t> export_to_binary() const;
};


block::ref MiyaCoreMSG_BLOCK::get_block()
{
  return _block;
}


}


#endif // A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
