#ifndef FF2315BE_72BA_4E68_ABDA_0E25FE3972F3
#define FF2315BE_72BA_4E68_ABDA_0E25FE3972F3


#include <memory>
#include <vector>
#include <cassert>
#include <array>

#include <share/hash/sha_hash.h>
#include <share/binary_utils.hpp>


namespace core
{
	struct MiyaCoreContext;
};


namespace tx
{


struct coinbase_tx_in;
struct TxOut;



struct coinbase
{
private:
    struct Meta
	{
	  std::uint32_t _version;
	  std::shared_ptr<coinbase_tx_in> _tx_in;
	  std::shared_ptr<TxOut> _tx_out;
	} _meta;

	std::shared_ptr<unsigned char> _pubkeyHash; // txOutに渡す用(書き出しには使わない)
	std::shared_ptr<core::MiyaCoreContext> _mcContext;

public:
	coinbase( unsigned int height /*inに埋め込む*/ , std::shared_ptr<unsigned char> text , unsigned int textLength , std::shared_ptr<unsigned char> pubkeyHash , const std::shared_ptr<core::MiyaCoreContext> mcContext );
	coinbase();

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	size_t importRawSequentially( std::shared_ptr<unsigned char> from );
	size_t importRawSequentially( void *from );

	void add( std::shared_ptr<tx::TxOut> target );
	int height();

	std::shared_ptr<coinbase_tx_in> txIn();
	std::shared_ptr<TxOut> txOut();

	unsigned int calcTxID( std::shared_ptr<unsigned char> *ret ); // 一旦全てを書き出してハッシュするため頻繁には呼び出さないこと
	
	std::vector<std::uint8_t> export_to_binary() const;
};


};


#endif
