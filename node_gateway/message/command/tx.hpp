#ifndef C4EB1366_9BEB_4552_AB53_FE075A04AB1E
#define C4EB1366_9BEB_4552_AB53_FE075A04AB1E


#include "./command_utils.hpp"


namespace chain
{


struct MiyaCHainMSG_TX
{
private:
	// 生のブロックデータを送信する

public:
  static constexpr char command[12] = "tx";

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


}; // namespace chain


#endif // C4EB1366_9BEB_4552_AB53_FE075A04AB1E



