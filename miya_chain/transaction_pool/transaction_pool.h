#ifndef E7B29E63_DA7D_4286_B8DD_176AD41051AC
#define E7B29E63_DA7D_4286_B8DD_176AD41051AC


#include <memory>




namespace tx
{
	class P2PKH;
}



namespace miya_chain
{


struct TxCB;
class TxCBTable;
class P2PKH;



class TransactionPool
{

private:
	TxCBTable *_rootTable;

public:
	TransactionPool();

	//std::shared_ptr<tx::P2PKH> find( std::shared_ptr<tx::P2PKH>  );
	std::shared_ptr<TxCB> find( std::shared_ptr<tx::P2PKH> target );
	void store( std::shared_ptr<tx::P2PKH> target );
};




};

#endif //


