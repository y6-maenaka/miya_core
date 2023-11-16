#include "utxo.h"


#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/tx/tx_out.h"

#include "../transaction/script/script.h"
#include "../transaction/script/pk_script.h"

#include "../../shared_components/cipher/ecdsa_manager.h"


namespace miya_chain
{






std::vector<uint8_t> UTXO::dumpToBson()
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
	//retJson["used"] = isUsed;


	std::vector<uint8_t> ret;
	ret = nlohmann::json::to_bson( retJson );

	return ret;
}


size_t UTXO::dumpToBson( std::shared_ptr<unsigned char> *retRaw )
{
	std::vector<uint8_t> dumpedUTXOVector = this->dumpToBson();

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedUTXOVector.size()] );
	std::copy( dumpedUTXOVector.begin() , dumpedUTXOVector.end() , (*retRaw).get() );

	return dumpedUTXOVector.size();
}


size_t UTXO::exportRaw( std::shared_ptr<unsigned char> *retRaw ) // 未検証
{
	if( _txOut == nullptr ){
		*retRaw = nullptr;
		return 0;
	}

	// 先ずpkScriptを書き出しておく
	std::shared_ptr<unsigned char> exportedRawPkScript; size_t exportedRawPkScriptLength;
	exportedRawPkScriptLength = _content._pkScript->exportRawWithP2PKHPkScript( &exportedRawPkScript , _txOut->pubKeyHash() );

	// 書き出しサイズ
	size_t retRawLength = sizeof(_content._txID) + sizeof(_content._outputIndex) + sizeof(_content._amount) + exportedRawPkScriptLength;
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );

	size_t formatPtr = 0;
	memcpy( (*retRaw).get() + formatPtr , &(_content._txID) , sizeof(_content._txID) );  // txIDの書き出し
	formatPtr += sizeof(_content._txID);

	memcpy( (*retRaw).get() + formatPtr , &(_content._outputIndex), sizeof(_content._outputIndex) ); // outputIndexの書き出し
	formatPtr += sizeof(_content._outputIndex);

	memcpy( (*retRaw).get() + formatPtr , &(_content._amount) , sizeof(_content._amount) ); // amoutの書き出し
	formatPtr += sizeof(_content._amount);

	memcpy( (*retRaw).get() + formatPtr , exportedRawPkScript.get() , exportedRawPkScriptLength );
	formatPtr += exportedRawPkScriptLength;

	return formatPtr;
}



UTXO::UTXO( std::shared_ptr<tx::TxOut> target, uint32_t index )
{
	_txOut = target;
	_content._txID = std::shared_ptr<unsigned char>( new unsigned char[32] ); // あとで定数に置き換える
	_content._outputIndex = index;
	_content._pkScript = std::shared_ptr<tx::PkScript>( new tx::PkScript );
}


size_t UTXO::amount()
{
	return static_cast<size_t>(ntohll(_content._amount));
}


std::shared_ptr<tx::PkScript> UTXO::pkScript()
{
	return _content._pkScript;
}


bool UTXO::importFromBson( nlohmann::json fromBson )
{
	if( !(fromBson.contains("value")) ) return false;

	nlohmann::json valueJson = nlohmann::json::from_bson( fromBson["value"].get_binary() );
	return this->importFromBson( valueJson );
}


bool UTXO::importFromJson( nlohmann::json fromJson )
{
	//if( !(fromJson.contains("used")) ) return false;
	//_content._used = fromJson["used"];

	if( !(fromJson.contains("TxID")) ) return false;
	if( !(fromJson["TxID"].is_binary()) )  return false;
	std::vector<uint8_t> txIDVector = fromJson["TxID"].get_binary();

	_content._txID = std::shared_ptr<unsigned char>( new unsigned char[txIDVector.size()] );
	std::copy( txIDVector.begin() , txIDVector.begin() + txIDVector.size(), _content._txID.get() );

	if( !(fromJson.contains("index")) ) return false;
	_content._outputIndex = static_cast<uint32_t>(fromJson["index"]);

	if( !(fromJson.contains("amount")) )	return false;
	_content._amount = static_cast<uint64_t>(fromJson["amount"]);

	if( !(fromJson.contains("PkScript")) ) return false;
	if( !(fromJson["PkScript"].is_binary()) ) return false;
	std::vector<uint8_t> pkScriptVector = fromJson["PkScript"].get_binary();
	std::shared_ptr<unsigned char> rawPkScript = std::shared_ptr<unsigned char>( new unsigned char[pkScriptVector.size()] );
	std::copy( pkScriptVector.begin() , pkScriptVector.begin() + pkScriptVector.size() , rawPkScript.get() );
	_content._pkScript = std::make_shared<tx::PkScript>();
	_content._pkScript->importRaw( rawPkScript.get() , pkScriptVector.size() );

	return true;
}









}
