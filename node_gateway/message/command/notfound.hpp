#ifndef D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69
#define D4B0F9C9_EE7C_4DF0_AF87_144493AAFF69


#include "./base_command.hpp"
#include "./inv/inv.hpp"


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
    static constexpr char command[12] = "notfound";

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


}; // namespace chain


#endif
