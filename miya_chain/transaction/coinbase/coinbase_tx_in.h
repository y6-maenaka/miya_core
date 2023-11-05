#ifndef E2E78A29_DE9B_4D35_93AD_85DF95A28046
#define E2E78A29_DE9B_4D35_93AD_85DF95A28046


#include <memory>




namespace tx
{


struct PrevOut;
class Script;




struct CoinbaseTxIn
{
private:
			
	struct Body// __attribute__((packed))
	{
		std::shared_ptr<PrevOut> _prevOut; // 36 bytes
		uint32_t _script_bytes; // unLockingScriptのバイト長 // 40
		std::shared_ptr<Script>	_script;
		uint32_t _sequence;

	} _body;

public:
	CoinbaseTxIn();
	CoinbaseTxIn( uint32_t height , std::shared_ptr<unsigned char> text , unsigned int textLength );

	std::shared_ptr<PrevOut> prevOut();
	unsigned short scriptBytes();

	int height();

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	unsigned int importRaw( unsigned char *fromRaw );
};



};





#endif // 
