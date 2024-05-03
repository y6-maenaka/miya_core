#ifndef DA4E26FE_C166_43AD_9623_B65C14DDB047
#define DA4E26FE_C166_43AD_9623_B65C14DDB047


#include <iostream>
#include <chrono>
#include <variant>
#include <memory>

#include <chain/transaction/p2pkh/p2pkh.h>


namespace tx
{

struct P2PKH;
//class Tx;

};

namespace chain
{


constexpr unsigned int TX_ID_LENGTH = (160 / 8);
//using Tx = std::variant< std::shared_ptr<tx::P2PKH> >;


struct TxCB
{
private:
	struct 
	{
		std::shared_ptr<unsigned char> _txID;
		int _status; // 0 : for store , 1 : for search
		std::time_t _timestamp;
	} _meta;

	std::shared_ptr<tx::P2PKH> _tx;

	struct 
	{
		std::shared_ptr<TxCB> _leastChain = nullptr;
		std::shared_ptr<TxCB> _prevCB = nullptr;
		std::shared_ptr<TxCB> _nextCB = nullptr;
		int _priority = 0;
		
	} _control;


public:
	TxCB( int status = 1 ){ _meta._status = status; };
	//TxCB( std::shared_ptr<unsigned char> txID , int status = 1 );
	TxCB( std::shared_ptr<tx::P2PKH> target , int status = 0 ); // set tx

	std::shared_ptr<TxCB> prev();
	std::shared_ptr<TxCB> next();

	void prev( std::shared_ptr<TxCB> target );
	void next( std::shared_ptr<TxCB> target );

	std::shared_ptr<tx::P2PKH> tx();
	//unsigned char* txID();
	std::shared_ptr<unsigned char> txID();
	void txID( std::shared_ptr<unsigned char> target ); // テスト用 基本的には外部からの変更は不可能にする


	int priority();
	void priority( int priority );
};


};


#endif // DA4E26FE_C166_43AD_9623_B65C14DDB047

