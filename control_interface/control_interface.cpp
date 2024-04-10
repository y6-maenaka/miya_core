#include "control_interface.h"

void ControlInterface::start()
{
	std::string instructionsFilePath = "../control_interface/inst/pattern1.json";
		
	std::ifstream input(instructionsFilePath);
	nlohmann::json instructionsFrom;
	input >> instructionsFrom;


	for(int i=0;i<instructionsFrom["instructions"].size();i++)
	{
		std::string contentString = instructionsFrom["instructions"][i]["content"];
		std::cout << contentString.size()  << "\n";
		std::shared_ptr<unsigned char> content = std::shared_ptr<unsigned char>( new unsigned char[contentString.size()] ); 
		std::copy( contentString.begin() , contentString.end(), content.get() );

		command_exe( instructionsFrom["instructions"][i]["command"] , content );
	}
}




void ControlInterface::command_exe( std::string command , std::shared_ptr<unsigned char> rawConetnt )
{
	std::unique_ptr<SBSegment> sb = std::make_unique<SBSegment>();
	if( command == COMMAND_TO_SENDER )	
	{
		std::cout << "COMMAND_TO_SENDER done" << "\n";
	}

	if( command == COMMAND_TO_RECEIVER ) 
	{
		std::cout << "COMMAND_TO_RECEIVER done" << "\n";

	}

	if( command == COMMAND_TO_K_ROUTING_TABLE_UPDATOR )
	{
		std::cout << "COMMAND_TO_UPDATOR done" << "\n";
	}

	return;
}






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




void ControlInterface::toSenderSB( std::shared_ptr<StreamBufferContainer> target )
{
	_ekp2pMainDaemons._toSenderSB = target;
}
