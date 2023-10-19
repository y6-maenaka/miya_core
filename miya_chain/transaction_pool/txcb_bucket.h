#ifndef FF2426D3_76BF_4721_A45D_576C0BA7B386
#define FF2426D3_76BF_4721_A45D_576C0BA7B386



#include <memory>
#include <unistd.h>
#include <string.h>



namespace miya_chain
{


//class TransactionPool;
class TxCBTable;
struct TxCB;



constexpr unsigned short DEFAULT_SCALE_SIZE = 3;



class TxCBBucket
{
public: // 将来的にprivateにする
	TxCBTable* _parentTable; // スケール用に自身のバケットが属するテーブルを記録しておく
	std::shared_ptr<TxCB> _txcbHead; // 先頭TxCBポインタ
	size_t _txCount = 0;

	unsigned int _defaultCapacity; // auto scaleの基準
	const unsigned char _bucketSymbol;
	unsigned short _scaleSize;


public:
	TxCBBucket( unsigned char symbol , TxCBTable* parentTable );

	int add( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCB> find( std::shared_ptr<TxCB> target ); 
	void remove( std::shared_ptr<TxCB> target );
	
	void autoScaleUp( std::shared_ptr<TxCB>txcbHead /* scaleHead */);
	void autoScaleDown(); // 最悪なくても大丈夫

	void printBucket();
};






};



#endif // FF2426D3_76BF_4721_A45D_576C0BA7B386



