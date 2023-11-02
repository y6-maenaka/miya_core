#ifndef E716B8ED_6FFD_44E0_A946_6BB2781AF8B8
#define E716B8ED_6FFD_44E0_A946_6BB2781AF8B8


#include "script.h"

#include <memory>
#include <functional>
#include <variant>
#include <iostream>
#include <map>
#include <string>



namespace tx
{


class PkScript;
class SignatureScript;
struct P2PKH;
class ValidationOptions;





struct ValidationStack
{
//private:
public:
	std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > > _body;

public:
	void pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data = nullptr );
	std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > popBack();

	int controlStack( const std::function<bool( ValidationStack* , ValidationOptions *optionsPtr )>& callback , ValidationOptions *optionsPtr = nullptr );
};





struct ValidationOptions
{
	std::pair< std::shared_ptr<unsigned char>, unsigned int > _txHash;
	std::pair< std::shared_ptr<unsigned char>, unsigned int > _opData;
};



class ScriptValidator
{
private:
	ValidationStack _stack;
	std::vector< std::pair< OP_CODES, std::shared_ptr<unsigned char> > > _joinedScript;
	ValidationOptions _options;


public:
	ScriptValidator( std::shared_ptr<PkScript> pkScript , std::shared_ptr<SignatureScript> signatureScript ); // 何の検証をするにしてもpkSとsigSは必要

	//void scriptControl( OPCodeCallback callback );
	bool verifyP2PKHScript( std::shared_ptr<unsigned char> txHash , unsigned int txHashLength , bool showHistory = true );

		
};













};


#endif// 


