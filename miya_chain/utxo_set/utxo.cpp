#include "utxo.h"


#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/tx/tx_out.h"

#include "../transaction/script/script.h"
#include "../transaction/script/pk_script.h"

#include "../../shared_components/cipher/ecdsa_manager.h"


namespace miya_chain
{






std::vector<uint8_t> UTXO::dumpToBson( bool isUsed )
{
	nlohmann::json retJson;

	std::vector<uint8_t> txIDVector; txIDVector.assign( _content._txID.get() , _content._txID.get() + 32 );
	retJson["TxID"] = nlohmann::json::binary( txIDVector );

	retJson["index"] = _content._outputIndex;

	retJson["amount"] = _content._amount;

	
	std::shared_ptr<unsigned char> exportedRawPkScript; size_t exportedRawPkScriptLength;
	exportedRawPkScriptLength = _txOut->pkScript()->exportRawWithP2PKHPkScript( &exportedRawPkScript , _txOut->pubKeyHash() );
	std::vector<uint8_t> pkScriptVector; pkScriptVector.assign( exportedRawPkScript.get() , exportedRawPkScript.get() + exportedRawPkScriptLength );
	retJson["PkScript"] = nlohmann::json::binary( pkScriptVector );
	

	retJson["used"] = isUsed;


	std::vector<uint8_t> ret;
	ret = nlohmann::json::to_bson( retJson );

	return ret;
}




UTXO::UTXO( std::shared_ptr<tx::TxOut> target )
{
	_txOut = target;
}





void UTXO::set( std::shared_ptr<tx::TxOut> target , std::shared_ptr<unsigned char> txID , unsigned short index )
{

}




bool UTXO::set( nlohmann::json dbResponse )
{
	if( !(dbResponse.contains("value")) ) return false;


	std::vector<uint8_t> valueVector = dbResponse["value"].get_binary();
	nlohmann::json value = nlohmann::json::from_bson( valueVector );


	if( !(value.contains("used")) ) return false;
	_content._used = value["used"];


	if( !(value.contains("TxID")) ) return false;
	if( !(value["TxID"].is_binary()) )  return false;
	std::vector<uint8_t> txIDVector = value["TxID"].get_binary();
	
	_content._txID = std::shared_ptr<unsigned char>( new unsigned char[txIDVector.size()] );
	std::copy( txIDVector.begin() , txIDVector.begin() + txIDVector.size(), _content._txID.get() );


	if( !(value.contains("index")) ) return false;
	_content._outputIndex = static_cast<uint32_t>(value["index"]);


	if( !(value.contains("amount")) )	return false;
	_content._amount = static_cast<uint64_t>(value["amount"]);

	
	if( !(value.contains("PkScript")) ) return false;
	if( !(value["PkScript"].is_binary()) ) return false;
	std::vector<uint8_t> pkScriptVector = value["PkScript"].get_binary();
	std::shared_ptr<unsigned char> rawPkScript = std::shared_ptr<unsigned char>( new unsigned char[pkScriptVector.size()] );
	std::copy( pkScriptVector.begin() , pkScriptVector.begin() + pkScriptVector.size() , rawPkScript.get() );
	_content._pkScript = std::make_shared<tx::PkScript>();
	_content._pkScript->importRaw( rawPkScript.get() , pkScriptVector.size() );


	return true;
}




}
