#ifndef FF2315BE_72BA_4E68_ABDA_0E25FE3972F3
#define FF2315BE_72BA_4E68_ABDA_0E25FE3972F3


#include <memory>
#include <vector>


namespace tx
{


struct TxIn;
struct TxOut;



struct Coinbase
{
//private:
public:
    struct 
		{
      int32_t _version;
      std::shared_ptr<TxIn> _txIn;
			std::vector< std::shared_ptr<TxOut> > _txOuts;
    } _body;

//public:
	Coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength );

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
};







};



#endif 
