#ifndef E2E78A29_DE9B_4D35_93AD_85DF95A28046
#define E2E78A29_DE9B_4D35_93AD_85DF95A28046


#include <memory>
#include <vector>


namespace tx
{


struct prev_out;
class Script;


struct coinbase_tx_in
{
private:
	struct Body// __attribute__((packed))
	{
		std::shared_ptr<prev_out> _prevOut; // 36 bytes
		std::uint32_t _script_bytes; // unLockingScriptのバイト長 // 40
		std::shared_ptr<Script>	_script;
		std::uint32_t _sequence;

	} _body;

public:
	coinbase_tx_in();
	coinbase_tx_in( uint32_t height , std::shared_ptr<unsigned char> text , unsigned int textLength );

	std::shared_ptr<prev_out> prevOut();
	unsigned short scriptBytes();

	int height();

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	unsigned int importRaw( unsigned char *fromRaw );

	std::vector<std::uint8_t> export_to_binary() const;
};


};


#endif // 
