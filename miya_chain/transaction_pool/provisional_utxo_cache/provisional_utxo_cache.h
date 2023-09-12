#ifndef db54e80e_2621_11a8_5ccb_38d6bf841000
#define db54e80e_2621_11a8_5ccb_38d6bf841000




#include <map>
#include <unordered_map>
#include <memory>
#include <utility>



/*
 utxoを一意に特定する要素
	
	<utxo>
	|- TxID
	|- OutputIndex
	|- Amout
	|- Address
	|- Script

	-----------------------------
	⭐️ TxID ( 32 bytes )
	⭐️ OutputIndex ( 4 bytes )
	-----------------------------

 */



namespace tx
{
	class P2PKH;
}



namespace miya_chain
{



struct TxCB;



using OutPointPair = std::pair< 
																std::shared_ptr<unsigned char>,  // トランザクションID
																uint32_t   // トランザクション無いインデックス
															>;


struct CustomCompare
{ // カスタム比較関数
	bool operator ()( std::shared_ptr<OutPointPair> left, std::shared_ptr<OutPointPair> right ) const  // 第1引数が小さい場合にtrueが帰るように
	{
			int ret;
			if( (ret = memcmp( left->first.get() , right->first.get() , 32 ))  == 0 )
				return (left->second < right->second);
			return (ret < 0);
	}
};






// トランザクションプール内にストックされているtxが参照しているutxo(暫定)が管理されているマップ
class ProvisionalUTxOCache // 暫定utxoキャッシュ -> トランザクションプール内のトランザクションが参照してるutxoのマップ
{
private:
	std::map< std::shared_ptr<OutPointPair> , std::shared_ptr<TxCB>, CustomCompare > _pUTxOMap; // Provisional_UTXO_Map

public:
	std::shared_ptr<TxCB> find( std::shared_ptr<unsigned char> txID , unsigned short index );
	std::vector< std::shared_ptr<TxCB> > find( std::shared_ptr<tx::P2PKH> target );

	// 該当するトランザクションを全て削除する必要がある
	/* 単体削除 */
	void remove( std::shared_ptr<unsigned char> txID , unsigned short index ); 
	void remove( std::shared_ptr<OutPointPair> targetPair  ); 
	/* バッチ削除 */
	void remove( std::shared_ptr<TxCB> target ); // トランザクション分をまとめて削除する


	// 基本的にトランザクションプールにtxが追加された際にprovisionalUTxOCacheも追加される
	void add( std::shared_ptr<TxCB> target ); // txに含まれているtx_outを全て追加する
};







};


#endif // db54e80e_2621_11a8_5ccb_38d6bf841000
