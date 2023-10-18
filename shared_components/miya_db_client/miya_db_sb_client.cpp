#include "miya_db_sb_client.h"


#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/json.hpp"

#include "./common.h"




MiyaDBSBClient::MiyaDBSBClient( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer )
{
	if( _popSBContainer == nullptr ) return;
	if( _pushSBContainer == nullptr ) return;

	_popSBContainer = popSBContainer;
	_pushSBContainer = pushSBContainer;

	return;
}






size_t MiyaDBSBClient::get( std::shared_ptr<unsigned char> rawKey ,std::shared_ptr<unsigned char> *retRaw )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ){
		*retRaw = nullptr;
		return 0;
	}


	std::vector<uint8_t> keyVector;
	keyVector.assign( rawKey.get() , rawKey.get() + 32 );

	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_GET;
	queryJson["Key"] = nlohmann::json::binary( keyVector );

	std::vector<uint8_t> dumpedQueryVector; 
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );

	std::shared_ptr<unsigned char> dumpedQueryRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin(), dumpedQueryVector.end() , dumpedQueryRaw.get() );

	// 検索キーはどうする
	std::unique_ptr<SBSegment> querySB = std::unique_ptr<SBSegment>();
	querySB->body( dumpedQueryRaw, dumpedQueryVector.size() ); 

	_pushSBContainer->pushOne( std::move(querySB) ); // Request


	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;){
		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") &&  (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}

	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[responseVector.size()] );
	std::copy( responseVector.begin(), responseVector.end(), (*retRaw).get() );
	
	return responseVector.size();
}







bool MiyaDBSBClient::add( std::shared_ptr<unsigned char> rawKey , std::shared_ptr<unsigned char> rawValue , size_t rawValueLength )
{
	if( _popSBContainer == nullptr || _popSBContainer == nullptr ) return false;

	std::vector<uint8_t> keyVector;
	keyVector.assign( rawKey.get() , rawKey.get() + 32 );

	std::vector<uint8_t> valueVector;
	valueVector.assign( rawValue.get() , rawValue.get() + rawValueLength );


	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_GET;
	queryJson["Key"] = nlohmann::json::binary( keyVector );
	queryJson["Value"] = nlohmann::json::binary( valueVector );

	std::vector<uint8_t> dumpedQueryVector; 
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );

	std::unique_ptr<SBSegment> querySB = std::unique_ptr<SBSegment>();
	_pushSBContainer->pushOne( std::move(querySB) ); // Request


	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;){
		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") &&  (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );  // 違うSBセグメントは一旦キュー最後尾に送っておく
	}

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == 0);
}


