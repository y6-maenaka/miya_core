#include "query_parser.h"

#include "../database_manager.h"




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






std::shared_ptr<QueryContext> QueryParser::parseQuery( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	std::vector<uint8_t> fromBson;
	fromBson.assign( fromRaw.get() , fromRaw.get() + fromRawLength );

	// 一旦簡単に実装しておく
	nlohmann::json serializedQuery = nlohmann::json::from_bson( fromBson ); // ここで大量のエラーが発生する

	std::cout << "..................." << "\n";
	std::cout << serializedQuery["QueryID"] << "\n";
	std::cout << "..................." << "\n";

	if( !(serializedQuery.contains("query")) ) return nullptr;
	if( !(serializedQuery.contains("QueryID")) ) return nullptr;

	std::shared_ptr<QueryContext> queryContext = std::make_shared<QueryContext>( static_cast<int>(serializedQuery["query"]), static_cast<uint32_t>(serializedQuery["QueryID"]) );
	std::cout << "QueryContext::type :: " << queryContext->type() << "\n";
	switch( queryContext->type() )
	{
		case QUERY_ADD: // ADD
		{
			if( !(serializedQuery["key"].is_binary()) ) return nullptr;
			std::vector<uint8_t> keyVector;
			keyVector = serializedQuery["key"].get_binary();

			if( !(serializedQuery["value"].is_binary()) ) return nullptr;
			std::vector<uint8_t> valueVector;
			valueVector = serializedQuery["value"].get_binary();

			std::shared_ptr<unsigned char> value = std::shared_ptr<unsigned char>( new unsigned char[valueVector.size()] );
			std::copy( valueVector.begin() , valueVector.begin() + valueVector.size() , value.get() );
			queryContext->value( value , valueVector.size() );

			std::shared_ptr<unsigned char> key = std::shared_ptr<unsigned char>( new unsigned char[keyVector.size()] );
			std::copy( keyVector.begin() , keyVector.begin() + keyVector.size() , key.get() );
			queryContext->key( key , keyVector.size() );

			break;
		}

		case QUERY_SELECT: // GET
		{
			if( !(serializedQuery["key"].is_binary() ) ) return nullptr;

			std::vector<uint8_t> keyVector;
			keyVector = serializedQuery["key"].get_binary();

			std::shared_ptr<unsigned char> key = std::shared_ptr<unsigned char>( new unsigned char[keyVector.size()] );
			std::copy( keyVector.begin() , keyVector.begin() + keyVector.size() , key.get() );
			queryContext->key( key , keyVector.size() );

			break;
		}

		case QUERY_REMOVE: // remove
		{
			if( !(serializedQuery["key"].is_binary() ) ) return nullptr;

			std::vector<uint8_t> keyVector;
			keyVector = serializedQuery["key"].get_binary();
			std::shared_ptr<unsigned char> key = std::shared_ptr<unsigned char>( new unsigned char[keyVector.size()] );
			std::copy( keyVector.begin() , keyVector.begin() + keyVector.size() , key.get() );
			queryContext->key( key , keyVector.size() );

			break;
		}

		case QUERY_MIGRATE_SAFE_MODE:
		{
			break;
		}

		case QUERY_SAFE_MODE_COMMIT:
		{
			break;
		}

		case QUERY_SAFE_MODE_ABORT:
		{
			break;
		}

		default:
		{
			queryContext->type(-1);
		}
	};

	if( !(serializedQuery.contains("registryIndex") )) queryContext->registryIndex(-1);
	else queryContext->registryIndex( serializedQuery["registryIndex"] );
	std::cout << "queryContext retuend" << "\n";
	std::cout << "SafeModeRegistry :: " << queryContext->registryIndex() << "\n";
	return queryContext;
}





void QueryParser::hello()
{

}


};
