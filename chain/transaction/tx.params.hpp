#ifndef CF87E88F_B00B_4A48_A407_21E0FD8F5B1B
#define CF87E88F_B00B_4A48_A407_21E0FD8F5B1B


namespace tx
{

constexpr unsigned int TX_HASH_BITS_LENGTH = 256;
constexpr unsigned int TX_HASH_BYTES_LENGTH = TX_HASH_BITS_LENGTH / 8;

using tx_hash = std::array<std::uint8_t, TX_HASH_BYTES_LENGTH>;
using tx_id = tx_hash;


}; // namespace tx


#endif 
