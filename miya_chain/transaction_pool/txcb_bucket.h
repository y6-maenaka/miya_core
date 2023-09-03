#ifndef FF2426D3_76BF_4721_A45D_576C0BA7B386
#define FF2426D3_76BF_4721_A45D_576C0BA7B386



#include <memory>



namespace miya_chain
{


//class TransactionPool;
class TxCBTable;
struct TxCB;



constexpr unsigned short DEFAULT_SCALE_SIZE = 100;



class TxCBBucket
{
private:
	std::shared_ptr<TxCBTable> _parentTable; // スケール用に自身のバケットが属するテーブルを記録しておく
	std::shared_ptr<TxCB> _txcbHead; // 先頭TxCBポインタ

	unsigned int _defaultCapacity; // auto scaleの基準
	unsigned char _bucketSymbol;
	unsigned short _scaleSize;


public:
	TxCBBucket( unsigned char symbol , std::shared_ptr<TxCBTable> parentTable );

	int add( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCB> find( std::shared_ptr<TxCB> target );
	
	void autoScale( std::shared_ptr<TxCB>txcbHead );
};






};



#endif // FF2426D3_76BF_4721_A45D_576C0BA7B386



