#ifndef DA4E26FE_C166_43AD_9623_B65C14DDB047
#define DA4E26FE_C166_43AD_9623_B65C14DDB047


#include <iostream>
#include <chrono>


namespace tx
{

class P2PKH;

};







namespace miya_chain
{




constexpr unsigned int TX_ID_LENGTH = (160 / 8);





struct TxCB
{
private:

	struct 
	{
		//unsigned char* _txID;
		std::shared_ptr<unsigned char> _txID;
		int _status;
		std::time_t _timestamp;
	} _meta;

	std::shared_ptr<tx::P2PKH> _tx;


	struct 
	{
		std::shared_ptr<TxCB> _leastChain = nullptr;
		std::shared_ptr<TxCB> _prevCB = nullptr;
		std::shared_ptr<TxCB> _nextCB = nullptr;
		
	} _control;


public:
	TxCB( std::shared_ptr<tx::P2PKH> target ); // set tx

	std::shared_ptr<TxCB> prev();
	std::shared_ptr<TxCB> next();

	void prev( std::shared_ptr<TxCB> target );
	void next( std::shared_ptr<TxCB> target );

	std::shared_ptr<tx::P2PKH> tx();
	//unsigned char* txID();
	std::shared_ptr<unsigned char> txID();
};




};

#endif // DA4E26FE_C166_43AD_9623_B65C14DDB047



