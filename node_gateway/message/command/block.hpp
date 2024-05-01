#ifndef A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
#define A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF


// #include <chain/block/block.h>
#include <memory>


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
  std::shared_ptr<struct Block> _block;

public:
  static constexpr char command[12] = "block";

  std::shared_ptr<unsigned char> blockHash();
  std::shared_ptr<struct Block> block();

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ){};
  bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ){};
  
  std::vector<std::uint8_t> export_to_binary() const;
};


}


#endif // A73A3F2F_60E3_40A4_8DF5_CB513B2E24BF
