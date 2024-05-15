#ifndef C966EC63_E11D_4BD3_BD98_0C867CEC0753
#define C966EC63_E11D_4BD3_BD98_0C867CEC0753


#include <array>
#include <memory>



namespace tx
{


struct P2PKH;


using transaction_ref = std::shared_ptr<const struct P2PKH>; // 一旦仮でP2PKHのみ
using const_transaction_ref = std::shared_ptr<const struct P2PKH>;

template < typename Tx > 
static inline transaction_ref make_transaction_ref( Tx && target ) // トランザクションオブジェクトの変換
{
  return std::make_shared<const transaction_ref>(std::forward<Tx>(target));
}


};


#endif 
