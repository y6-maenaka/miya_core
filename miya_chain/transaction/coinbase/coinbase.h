#ifndef FF2315BE_72BA_4E68_ABDA_0E25FE3972F3
#define FF2315BE_72BA_4E68_ABDA_0E25FE3972F3


#include <memory>
#include <vector>
#include <cassert>



namespace miya_core
{
	struct MiyaCoreContext;
};


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

	std::shared_ptr<unsigned char> _pubkeyHash; // txOutに渡す用
	std::shared_ptr<miya_core::MiyaCoreContext> _mcContext;

public:
	Coinbase( unsigned int height /*inに埋め込む*/ , std::shared_ptr<unsigned char> text , unsigned int textLength , std::shared_ptr<unsigned char> pubkeyHash , const std::shared_ptr<miya_core::MiyaCoreContext> mcContext );
	Coinbase();

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	size_t importRawSequentially( std::shared_ptr<unsigned char> from );
	size_t importRawSequentially( void *from );

	void add( std::shared_ptr<tx::TxOut> target );
	int height();

	std::shared_ptr<CoinbaseTxIn> txIn();
	std::shared_ptr<TxOut> txOut();

	unsigned int calcTxID( std::shared_ptr<unsigned char> *ret ); // 一旦全てを書き出してハッシュするため頻繁には呼び出さないこと
};







};



#endif 
