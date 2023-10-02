#include "query_parser.h"




namespace miya_db
{
	

UnifiedStrageManager *QueryParser::suggestTable()
{

	/*	
	switch( command ) // すでにメモリ上にテーブルが展開されていたら,それを返す
	{
		case 1:		
			UnifiedStrageManager *MMyISAMStrageManager = new MMyISAM; 
			return MMyISAMStrageManager;

			break;

		default:
			break;

	}

	return nullptr;
	*/
	return nullptr;
}






std::shared_ptr<QueryContext> parseQuery( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	std::vector<uint8_t> fromBson;
	fromBson.assign( fromRaw.get() , fromRaw.get() + fromRawLength );

	// 一旦簡単に実装しておく
	nlohmann::json serializedQuery = nlohmann::json::from_bson( fromBson ); // ここで大量のエラーが発生する


	if( !(serializedQuery.contains("query")) )
		return nullptr;

	if(  !(serializedQuery.contains("query")) || !(serializedQuery["query"].is_binary()) )
		return nullptr;



	std::shared_ptr<QueryContext>	queryContext = std::make_shared<QueryContext>( serializedQuery["query"] );

	switch( static_cast<int>(serializedQuery["query"]) )
	{

		case 1: // add
		{
			if( !(serializedQuery["value"].is_binary()) ) return nullptr;
			std::vector<uint8_t> valueVector; 
			valueVector = serializedQuery["value"].get_binary();


			std::shared_ptr<unsigned char> value = std::shared_ptr<unsigned char>( new unsigned char[valueVector.size()] );

			//std::copy( queryData.begin(), queryData.begin() + queryData.size(), data.get() );
			std::copy( valueVector.begin() , valueVector.begin() + valueVector.size() , value.get() );
			queryContext->value( value , valueVector.size() );
			break;
		}

		case 2: // find
		{
			if( !(serializedQuery["key"].is_binary() ) ) return nullptr;
			std::vector<uint8_t> keyVector;
			keyVector = serializedQuery["key"].get_binary();

			std::shared_ptr<unsigned char> key = std::shared_ptr<unsigned char>( new unsigned char[keyVector.size()] ); 

			std::copy( keyVector.begin() , keyVector.begin() + keyVector.size() , key.get() );
			queryContext->key( key , keyVector.size() );
			break;
		}
	};


	return queryContext;
}





void QueryParser::hello()
{

}


};
