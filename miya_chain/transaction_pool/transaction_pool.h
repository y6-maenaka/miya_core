#ifndef E7B29E63_DA7D_4286_B8DD_176AD41051AC
#define E7B29E63_DA7D_4286_B8DD_176AD41051AC


#include <memory>
// #include <unordered_map>
#include <map>
#include <vector>




namespace tx
{
	class P2PKH;
}



namespace miya_chain
{


struct TxCB;
class TxCBTable;
class P2PKH;
class ProvisionalUTxOCache; // トランザクションプール内にストックされているtxが参照しているutxo(暫定)が管理されているマップ




class TransactionPool
{

private:
	TxCBTable *_rootTable;
	ProvisionalUTxOCache *_pUTxOCache; // utxoと異なり,トランザクションプールに存在する分しか格納しないため,mapで妥協・許容する

public:
	TransactionPool();

	//std::shared_ptr<tx::P2PKH> find( std::shared_ptr<tx::P2PKH>  );
	std::shared_ptr<TxCB> find( std::shared_ptr<tx::P2PKH> target );
	void store( std::shared_ptr<tx::P2PKH> target );


	void remove( std::shared_ptr<TxCB> target ); // イテレータのeraseのようなメソッドにする
	void batchRemove( std::vector<std::shared_ptr<TxCB>> targetVector );
};




};

#endif //


