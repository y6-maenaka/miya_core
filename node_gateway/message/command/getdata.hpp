#ifndef BE75CE29_95AE_41A8_A9D7_39C43A973FC0
#define BE75CE29_95AE_41A8_A9D7_39C43A973FC0

#include "./inv.hpp"
#include <chain/block/block.hpp>
#include <memory>

namespace chain
{

/*
   [ 要求 ](複数)
    invメッセージで受信したオブジェクトのリストの中から特定のデータオブジェクトをリモートノードに要求する
    要求可能なデータはメモリープールまたはリレーセットにあるデータのみ
    ※ブロックに格納されたトランザクションデータにはアクセスできない
*/

struct MiyaCoreMSG_GETDATA 
{
private:
	MiyaCoreMSG_INV _inv;

public:
  using ref = std::shared_ptr<MiyaCoreMSG_GETDATA>;

  MiyaCoreMSG_GETDATA();
  template< const block::id... Args > MiyaCoreMSG_GETDATA( Args... args );
  MiyaCoreMSG_GETDATA( const std::vector< inv::ref > &invs );
  static constexpr char command[12] = "getdata";

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw ){};
  bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ){};

  struct MiyaCoreMSG_INV& get_inv();

  void inv( struct MiyaCoreMSG_INV target );
  void __print();


  std::vector<std::uint8_t> export_to_binary() const;
};


template< const block::id... Args > MiyaCoreMSG_GETDATA::MiyaCoreMSG_GETDATA( Args... args )
{
  for( const auto& block_id : std::initializer_list<const block::id>{args...} )
    _inv.add( inv(std::make_pair(inv::type_id::MSG_BLOCK, block_id)) );
}

MiyaCoreMSG_GETDATA::MiyaCoreMSG_GETDATA( const std::vector< inv::ref > &invs )
{
  return;
}

struct MiyaCoreMSG_INV& MiyaCoreMSG_GETDATA::get_inv()
{
  return _inv;
}

void MiyaCoreMSG_GETDATA::inv( struct MiyaCoreMSG_INV target )
{
	_inv = target;
}

void MiyaCoreMSG_GETDATA::__print()
{
	_inv.__print();
}


}; // namespace chain


#endif // BE75CE29_95AE_41A8_A9D7_39C43A973FC0
