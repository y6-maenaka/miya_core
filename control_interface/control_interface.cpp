#include "control_interface.h"

#include <fstream>
#include <iostream>
#include <unistd.h>


#include "../shared_components/json.hpp"

#include "../miya_chain/transaction/p2pkh/p2pkh.h"
#include "../miya_chain/transaction/tx/tx_in.h"
#include "../miya_chain/transaction/tx/tx_out.h"

#include "../miya_chain/transaction/script/signature_script.h"
#include "../miya_chain/transaction/script/pk_script.h"
#include "../miya_chain/transaction/script/script.h"





std::shared_ptr<tx::P2PKH> ControlInterface::createTxFromJsonFile( const char *filePath )
{
	std::cout << "Creating Transaction from JSON Data.." << "\n";

	std::ifstream input(filePath);
	if( !input.is_open() )
	{
		std::cout << "指定した送金関連ファイルを開くことができません : " << filePath << "\n";
		return nullptr;
	}

	nlohmann::json paymentData;
	input >> paymentData; // jsonファイルの取り込み


	//std::shared_ptr<tx::P2PKH> loadedP2PKH = std::make_shared<tx::P2PKH>();
	tx::P2PKH loadedP2PKH;


	// 送金先のトランザクション情報作成
	nlohmann::json tos = paymentData["destination"];
	for( auto itr : tos )
	{
		std::shared_ptr< tx::TxOut > newTxOut = std::make_shared<tx::TxOut>();
		*newTxOut = itr;
		loadedP2PKH.add( newTxOut );
	}
	std::cout << "\033[34m"  << "Loading Successfuly Done TxOut (" << loadedP2PKH.outCount() << ")" << "\033[0m" << "\n";


	std::shared_ptr<unsigned char> rawTxOut; unsigned int rawTxOutLength;


	// 送金元のトランザクション情報作成
	nlohmann::json froms = paymentData["source"];
	for( auto itr : froms )
	{
		std::shared_ptr<tx::TxIn> newTxIn = std::make_shared<tx::TxIn>();
		*newTxIn = itr;
		loadedP2PKH.add( newTxIn );
	}
	std::cout  << "\033[34m" << "Loading Successfuly Done TxIn (" << loadedP2PKH.inCount() << ")" << "\033[0m" << "\n";

	return std::make_shared<tx::P2PKH>( loadedP2PKH );
}
