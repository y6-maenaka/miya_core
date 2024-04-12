#include "miya_db_sb_client.h"


#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"


#include "./common.h"




MiyaDBSBClient::MiyaDBSBClient( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer )
{

	if( popSBContainer == nullptr ) return;
	if( pushSBContainer == nullptr ) return;

	_popSBContainer = popSBContainer;
	_pushSBContainer = pushSBContainer;

	std::cout << "MiyaDB Client setup done" << "\n";

	return;
}


size_t MiyaDBSBClient::get( std::shared_ptr<unsigned char> rawKey ,std::shared_ptr<unsigned char> *retRaw , short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ){
		*retRaw = nullptr;
		return 0;
	}

	std::vector<uint8_t> keyVector;
	keyVector.assign( rawKey.get() , rawKey.get() + 20 );

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_GET;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["registryIndex"] = registryIndex;

	std::vector<uint8_t> dumpedQueryVector; 
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );

	std::shared_ptr<unsigned char> dumpedRawQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin(), dumpedQueryVector.end() , dumpedRawQuery.get() );

	// 検索キーはどうする
	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery, dumpedQueryVector.size() ); 

	_pushSBContainer->pushOne( std::move(querySB) ); // Request

	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;){
		responseSB = _popSBContainer->popOne();

		std::cout << "[MiyaDBClient] Query poped" << "\n";
		
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") &&  (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}
	if( !(responseJson.contains("status")) ) return 0;
	if( responseJson["status"] != miya_db::MIYA_DB_STATUS_OK ){
		*retRaw = nullptr;
		return 0;
	}

	std::vector<uint8_t> valueVector = responseJson["value"].get_binary();
	//nlohmann::json valueJson = nlohmann::json::from_bson( responseJson["value"].get_binary() );
	std::cout << "[MiyaDBClient] responseSB->valueVector.size() :: " << valueVector.size() << "\n";
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[valueVector.size()] );
	std::copy( valueVector.begin(), valueVector.end(), (*retRaw).get() );

	return valueVector.size();
}


bool MiyaDBSBClient::add( std::shared_ptr<unsigned char> rawKey , std::shared_ptr<unsigned char> rawValue , size_t rawValueLength, short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	std::vector<uint8_t> keyVector;
	keyVector.assign( rawKey.get() , rawKey.get() + 20 );

	std::vector<uint8_t> valueVector;
	valueVector.assign( rawValue.get() , rawValue.get() + rawValueLength );

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_ADD;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["value"] = nlohmann::json::binary( valueVector );
	queryJson["registryIndex"] = registryIndex;

	std::vector<uint8_t> dumpedQueryVector; 
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );
	std::shared_ptr<unsigned char> dumpedRawQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin() , dumpedQueryVector.end(), dumpedRawQuery.get() );

	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery , dumpedQueryVector.size() );

	_pushSBContainer->pushOne( std::move(querySB) ); // Request

	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;

	for(;;){

		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") && (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}


	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == miya_db::MIYA_DB_STATUS_OK );
}


bool MiyaDBSBClient::remove( std::shared_ptr<unsigned char> rawKey, short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	std::vector<uint8_t> keyVector;
	keyVector.assign( rawKey.get() , rawKey.get() + 20 );

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_REMOVE;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["registryIndex"] = registryIndex;


	std::vector<uint8_t> dumpedQueryVector; 
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );
	std::shared_ptr<unsigned char> dumpedRawQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin() , dumpedQueryVector.end(), dumpedRawQuery.get() );

	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery , dumpedQueryVector.size() );

	_pushSBContainer->pushOne( std::move(querySB) ); // Request

	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;)
	{
		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") && (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == miya_db::MIYA_DB_STATUS_OK );
}


bool MiyaDBSBClient::safeMode( short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_SAFE_MODE;
	queryJson["registryIndex"] = registryIndex;

	std::cout << "ResigtryIndex(SBClient) :: " << queryJson["registryIndex"] << "\n";

	std::unique_ptr<SBSegment> querySB = generateQuerySB( queryJson );
	_pushSBContainer->pushOne( std::move(querySB) );

	nlohmann::json responseJson;
	responseJson = filterResponseSB( queryJson );

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == miya_db::MIYA_DB_STATUS_OK );
}


bool MiyaDBSBClient::abort( short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_ABORT;
	queryJson["registryIndex"] = registryIndex;

	std::unique_ptr<SBSegment> querySB = generateQuerySB( queryJson );
	_pushSBContainer->pushOne( std::move(querySB) );

	std::cout << "** 1" << "\n";
	nlohmann::json responseJson;
	std::cout << "** 2 " << "\n";
	responseJson = filterResponseSB( queryJson );
	std::cout << "** 3" << "\n";

	std::cout << responseJson << "\n";
	std::cout << "DB CLIENT STATUS :: " << responseJson["status"] << "\n";

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == miya_db::MIYA_DB_STATUS_OK );
}


bool MiyaDBSBClient::commit( short registryIndex )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_COMMIT;
	queryJson["registryIndex"] = registryIndex;

	std::unique_ptr<SBSegment> querySB = generateQuerySB( queryJson );
	_pushSBContainer->pushOne( std::move(querySB) );
	
	nlohmann::json responseJson;
	responseJson = filterResponseSB( queryJson );

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == miya_db::MIYA_DB_STATUS_OK );
}


size_t MiyaDBSBClient::dumpRaw( nlohmann::json queryJson , std::shared_ptr<unsigned char> *retRaw )
{
	std::vector<uint8_t> dumpedQueryVector;  // ret
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin() , dumpedQueryVector.end(), (*retRaw).get() );

	return dumpedQueryVector.size();
}



std::unique_ptr<SBSegment> MiyaDBSBClient::generateQuerySB( nlohmann::json queryJson )
{
	std::shared_ptr<unsigned char> dumpedRawQuery; size_t dumpedRawQueryLength;
	dumpedRawQueryLength =  dumpRaw( queryJson , &dumpedRawQuery );

	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery , dumpedRawQueryLength );
	
	return std::move(querySB);
}


nlohmann::json MiyaDBSBClient::filterResponseSB( nlohmann::json queryJson )
{
	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;)
	{
		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") && (responseJson["QueryID"] == queryJson["QueryID"]) )
			return responseJson;

		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}
}
