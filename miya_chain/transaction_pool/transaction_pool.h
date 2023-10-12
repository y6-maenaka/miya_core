#ifndef E7B29E63_DA7D_4286_B8DD_176AD41051AC
#define E7B29E63_DA7D_4286_B8DD_176AD41051AC


#include <memory>
// #include <unordered_map>
#include <map>
#include <vector>




namespace tx
{
	struct P2PKH;
}

namespace block
{
	struct Block;
}




namespace miya_chain
{


struct TxCB;
class TxCBTable;
class P2PKH;
class ProvisionalUTxOCache; // トランザクションプール内にストックされているtxが参照しているutxo(暫定)が管理されているマップ




// トランザクションの重複を許可しないメモリープール
class TransactionPool
{

//private:
public:
	// TxCBTable *_rootTable;
	std::shared_ptr<TxCBTable> _rootTable;
	// ProvisionalUTxOCache *_pUTxOCache; // utxoと異なり,トランザクションプールに存在する分しか格納しないため,mapで妥協・許容する

protected:
	std::shared_ptr<TxCB> find( std::shared_ptr<TxCB> target );

public:
	TransactionPool();

	//std::shared_ptr<tx::P2PKH> find( std::shared_ptr<tx::P2PKH>  );
	std::shared_ptr<TxCB> find( std::shared_ptr<tx::P2PKH> target );
	std::shared_ptr<TxCB>	find( std::shared_ptr<unsigned char> targetTxID );
	int add( std::shared_ptr<tx::P2PKH> target ); // トランザクションプール,暫定UTXO共に追加する
	int add( std::shared_ptr<TxCB> target );
	std::vector< std::shared_ptr<TxCB> > add( std::shared_ptr<block::Block> target , int priority = 0 );

	void remove( std::shared_ptr<TxCB> target ); // イテレータのeraseのようなメソッドにする
	void remove( std::shared_ptr<tx::P2PKH> target );
	void remove( std::shared_ptr<unsigned char> targetTxID );
	void remove( std::shared_ptr<block::Block> target );

	void batchRemove( std::vector<std::shared_ptr<TxCB>> targetVector );
	std::vector<TxCB> autoResolveDoubleSpends( std::shared_ptr<tx::P2PKH> target );

	static size_t bucketSymbolToIndex( const unsigned char symbol );
};




};

#endif //


