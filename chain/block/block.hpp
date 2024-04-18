#ifndef B5C82F86_2840_49C2_A60D_B47ECCF7B547
#define B5C82F86_2840_49C2_A60D_B47ECCF7B547


#include <array>


namespace chain
{


struct block // 今後はこっちを使う
{
public:
  using block_hash = std::array< std::uint8_t, BLOCK_HASH_BITS_LENGTH/8>;

private:
  // class block_header _header;
  // struct tx::Coinbase _coinbase;
};


};


#endif 


