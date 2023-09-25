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
	if( !(callback( this, optionsPtr )) ) return -1;
	return 0;
}

















ScriptValidator::ScriptValidator( std::shared_ptr<PkScript> pkScript , std::shared_ptr<SignatureScript> signatureScript )
{
	_joinedScript.insert( _joinedScript.end() ,pkScript->script()->begin()  , pkScript->script()->end() );
	_joinedScript.insert( _joinedScript.end() ,signatureScript->script()->begin()  , signatureScript->script()->end() );

}




bool ScriptValidator::verifyP2PKHScript( std::shared_ptr<unsigned char> txHash  , unsigned int txHashLength )
{
	int flag;

	int i=0;
	for( auto itr : _joinedScript )
	{
		/* まありよくない実装 */
		if( std::holds_alternative<OP_DUP>(itr.first) )
			flag = _stack.controlStack( OP_DUP::exe );

		else if( std::holds_alternative<OP_HASH_160>(itr.first) )
			flag = _stack.controlStack( OP_HASH_160::exe );

		else if( std::holds_alternative<OP_EQUALVERIFY>(itr.first) )
			flag = _stack.controlStack( OP_EQUALVERIFY::exe );

		else if( std::holds_alternative<OP_CHECKSIG>(itr.first) ){
			_options._txHash = std::make_pair( txHash , txHashLength );
			flag = _stack.controlStack( OP_CHECKSIG::exe ,  &_options );
		}

		else if( std::holds_alternative<OP_DATA>(itr.first) ){
			_options._opData = std::make_pair( itr.second, Script::OP_DATALength(itr.first) );
			flag = _stack.controlStack( OP_DATA::exe , &_options );
		}
		else
			continue;

		i++;
		if( flag != 0 ) return false;
	}
	return true;
}



}

