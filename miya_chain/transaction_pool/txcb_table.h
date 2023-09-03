#ifndef DDB39C1F_CDF1_451B_9160_516D7CC00583
#define DDB39C1F_CDF1_451B_9160_516D7CC00583


#include <map>
#include <array>



namespace miya_chain
{


struct TxCB;
class TxCBBucket;






class TxCBTable
{

private:
	unsigned int _layer;
	std::array< std::pair<void*,int> , 16 > _bucketTableList; // 下層にバケットもしくはテーブルを配置する
																									 // pair index [ 1 ] -> 下層にはバケットが存在する
																									 // pair index [ 2 ] -> 下層にテーブルが存在する

public:
	unsigned int layer();
	TxCBTable( unsigned int layer = 0 );

	int add( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCBBucket> deepestBucket( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCB> find( std::shared_ptr<TxCB> target );

	void bucketTableList( void* target , int type ,int index );
};







};

#endif // DDB39C1F_CDF1_451B_9160_516D7CC00583






