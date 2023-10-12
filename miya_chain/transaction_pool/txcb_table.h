#ifndef DDB39C1F_CDF1_451B_9160_516D7CC00583
#define DDB39C1F_CDF1_451B_9160_516D7CC00583


#include <map>
#include <array>
#include <variant>
#include <memory>
#include <iostream>



namespace miya_chain
{


struct TxCB;
class TxCBTable;
class TxCBBucket;




enum class TxTableOrBucket
{
	TX_Table ,
	TX_Bucket
};




//using TxTableBucket = std::variant< std::shared_ptr<TxCBBucket>
using TxCBContainer = std::variant< std::shared_ptr<TxCBTable> , std::shared_ptr<TxCBBucket> >;





class TxCBTable
{

//private:
public:
	const unsigned int _layer;
	std::array< TxCBContainer , 16u > _containerArray;
	//std::array< std::pair<void*,int> , 16 > _bucketTableList; // 下層にバケットもしくはテーブルを配置する
																									 // pair index [ 1 ] -> 下層にはバケットが存在する
																									 // pair index [ 2 ] -> 下層にテーブルが存在する

public:
	TxCBTable( unsigned int layer = 0 );

	int add( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCB> find( std::shared_ptr<TxCB> target );

	void remove( std::shared_ptr<TxCB> target );
	std::shared_ptr<TxCBBucket> findBucket( std::shared_ptr<TxCB> target );

	//void bucketTableList( void* target , int type ,int index );


	// re
	unsigned int layer();
	void txContainer( size_t index , TxCBContainer target );


	void printTable();
};







};

#endif // DDB39C1F_CDF1_451B_9160_516D7CC00583






