#include "utxo.h"

#include "../../shared_components/json.hpp"


namespace miya_chain
{






std::vector<uint8_t> UTXO::dumpToBson()
{

	nlohmann::json exportJson;


	/* txの書き出し */
	std::vector<uint8_t> txIDVector;
	txIDVector.assign( _txID.get() , _txID.get() + 32 ); // あとで変更する
	exportJson["txID"] = nlohmann::json::binary( txIDVector );

	/* indexの書き出し*/	
	exportJson["outputIndex"] = _outputIndex;

	/* amoutの書き出し */
	exportJson["amout"] = _amount;

	/*
	std::vector<uint8_t> addressVector;
	addressVector.assign( _address.get() , _address.get() + 32 );
	exportJson["address"] = nlohmann::json::binary( addressVector );
	*/	

	exportJson["used"] = false;

	return nlohmann::json::to_bson( exportJson );
}






bool UTXO::parseFromBson( std::vector<uint8_t> from )
{
	nlohmann::json importedJson;
	importedJson = nlohmann::json::from_bson( from );


	/* トランザクションIDの読み込み */
	std::vector<uint8_t> txIDVector;
	txIDVector = importedJson["txID"].get_binary();
	_txID = std::shared_ptr<unsigned char>( new unsigned char[txIDVector.size()] );
	std::copy( txIDVector.begin() , txIDVector.begin() + 32 , _txID.get() );

	_outputIndex = importedJson["outputIndex"];

	_amount = importedJson["amount"];


	/* アドレスの読み込み */
	/*
	std::vector<uint8_t> addressVector;
	addressVector = importedJson["address"].get_binary();
	_address = std::shared_ptr<unsigned char>( new unsigned char[addressVector.size()] );
	std::copy( addressVector.begin() , addressVector.begin() + 20 , _address.get() );
	*/

	_isUsed = importedJson["used"];	

	return true;	
}





void UTXO::set( std::shared_ptr<tx::TxOut> target )
{

}







}
