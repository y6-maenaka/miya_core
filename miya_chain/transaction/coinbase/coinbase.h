#ifndef FF2315BE_72BA_4E68_ABDA_0E25FE3972F3
#define FF2315BE_72BA_4E68_ABDA_0E25FE3972F3


#include <memory>
#include <vector>


namespace tx
{


struct TxIn;
struct TxOut;
struct CoinbaseTxIn;



struct Coinbase
{
private:
    struct 
		{
      int32_t _version;
			std::shared_ptr<CoinbaseTxIn> _txIn;
	  std::shared_ptr<TxOut> _txOut;
    } _body;

public:
	Coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength );
	Coinbase();

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	void importRaw( std::shared_ptr<unsigned char> from , unsigned int fromLength );

	void add( std::shared_ptr<tx::TxOut> target );
};







};



#endif 
