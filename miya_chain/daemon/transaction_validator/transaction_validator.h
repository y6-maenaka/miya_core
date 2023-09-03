#ifndef C9681570_EE7D_4BDE_8D4A_92926F0D9C48
#define C9681570_EE7D_4BDE_8D4A_92926F0D9C48



#include <memory>


namespace miya_chain
{


class TransactionPool;
class StreamBufferContainer;



class TransactionValidator
{
private:
	TransactionPool *_txPool;
	std::shared_ptr<StreamBufferContainer> _sourceSBC;


public:
	void start();

	TransactionValidator( TransactionPool *txPool , std::shared_ptr<StreamBufferContainer> sourceSBC );
};



};


#endif // 


