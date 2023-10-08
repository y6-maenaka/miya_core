#include "script_validator.h"


#include "./pk_script.h"
#include "./signature_script.h"


namespace tx
{









void ValidationStack::pushBack( OP_CODES opcode , std::shared_ptr<unsigned char> data )
{
	_body.push_back( std::make_pair( opcode, data ) );
}



std::shared_ptr< std::pair<OP_CODES, std::shared_ptr<unsigned char>> > ValidationStack::popBack()
{
	std::pair<OP_CODES, std::shared_ptr<unsigned char> > popedElem;

	popedElem = _body.back();
	_body.pop_back();

	return std::make_shared< std::pair<OP_CODES,std::shared_ptr<unsigned char>> >( popedElem );
}




int ValidationStack::controlStack( const std::function<bool( ValidationStack*, ValidationOptions *optionsPtr )>& callback , ValidationOptions *optionsPtr )
{
	std::cout << "controlStack called()" << "\n";
	if( !(callback( this, optionsPtr )) ) return -1;
	return 0;
}

















ScriptValidator::ScriptValidator( std::shared_ptr<PkScript> pkScript , std::shared_ptr<SignatureScript> signatureScript )
{
	//  逆？
	_joinedScript.insert( _joinedScript.end() ,signatureScript->script()->begin()  , signatureScript->script()->end() );
	_joinedScript.insert( _joinedScript.end() ,pkScript->script()->begin()  , pkScript->script()->end() );

	std::cout << "-------------------------" << "\n";
	printf("lockingScript seted >> %p\n", pkScript.get() );
	printf("unlockingScript seted >> %p\n", signatureScript.get() );
	printf("joinedScript size() -> %ld\n", _joinedScript.size() );
	std::cout << "-------------------------" << "\n";
}




bool ScriptValidator::verifyP2PKHScript( std::shared_ptr<unsigned char> txHash  , unsigned int txHashLength )
{
	int flag;

	std::cout << "--- 1 ---" << "\n";

	int i=0;
	for( auto itr : _joinedScript )
	{
		/* まありよくない実装 */
		if( std::holds_alternative<OP_DUP>(itr.first) ){
			std::cout << "OP_DUP execute()" << "\n";
			flag = _stack.controlStack( OP_DUP::exe );
			std::cout << "done" << "\n";
		}

		else if( std::holds_alternative<OP_HASH_160>(itr.first) ){
			std::cout << "OP_HASH_160 execute()" << "\n";
			flag = _stack.controlStack( OP_HASH_160::exe );
			std::cout << "done" << "\n";
		}

		else if( std::holds_alternative<OP_EQUALVERIFY>(itr.first) ){
			std::cout << "OP_EQUALVERIFY execute()" << "\n";
			flag = _stack.controlStack( OP_EQUALVERIFY::exe );
			std::cout << "done" << "\n";
		}

		else if( std::holds_alternative<OP_CHECKSIG>(itr.first) ){
			std::cout << "OP_CHECKSIG execute()" << "\n";
			_options._txHash = std::make_pair( txHash , txHashLength );
			flag = _stack.controlStack( OP_CHECKSIG::exe ,  &_options );
			std::cout << "done" << "\n";
		}

		else if( std::holds_alternative<OP_DATA>(itr.first) ){
			std::cout << "OP_DATA execute()" << "\n";
			_options._opData = std::make_pair( itr.second, Script::OP_DATALength(itr.first) );
			flag = _stack.controlStack( OP_DATA::exe , &_options );
			std::cout << "done" << "\n";
		}

		else{
			std::cout << "else execute()" << "\n";
			continue;
		}

		i++;
		if( flag != 0 ) return false;
	}
	return true;
}



}

