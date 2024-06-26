#include "utxo_set.h"


#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"
#include "../../share/json.hpp"
#include "../../share/hash/sha_hash.h"
#include "../../share/miya_db_client/common.h"
#include "../../share/miya_db_client/miya_db_sb_client.h"

#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/tx/tx_out.h"
#include "../transaction/tx/prev_out.h"

#include "./utxo.h"


namespace chain
{



LightUTXOSet::LightUTXOSet( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer )
{
	_pushSBContainer = pushSBContainer;
	_popSBContainer = popSBContainer;

	_miyaDBClient = std::make_shared<MiyaDBSBClient>( pushSBContainer , popSBContainer );
}


void LightUTXOSet::toVirtual()
{
	_miyaDBClient->safeMode();
}

void LightUTXOSet::virtualCommit()
{
	_miyaDBClient->commit();
}

void LightUTXOSet::virtualAbort()
{
	_miyaDBClient->abort();
}


std::shared_ptr<unsigned char> LightUTXOSet::generateUTxOKey( std::shared_ptr<unsigned char> txID , uint32_t index )
{
	size_t joinedIDIndexLength = 32 + sizeof(index);
	std::shared_ptr<unsigned char> joinedIDIndex = std::shared_ptr<unsigned char>( new unsigned char[joinedIDIndexLength] );
	memcpy( joinedIDIndex.get() , txID.get() , 32 );
	memcpy( joinedIDIndex.get() + 32 , &index , sizeof(index) );

	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKeyLength = hash::SHAHash( joinedIDIndex , joinedIDIndexLength, &rawKey , "sha1" );

	return rawKey;
}



std::shared_ptr<UTXO> LightUTXOSet::get( std::shared_ptr<unsigned char> txID , uint32_t index )
{
	//  キーを作成する
	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength = 20; // あとで修正する
	rawKey = generateUTxOKey( txID , index );
	std::vector<uint8_t> keyVector; keyVector.assign( rawKey.get() , rawKey.get() + rawKeyLength );

	std::cout << "\x1b[33m" << "-----------------------------------------------------------" << "\x1b[39m" << "\n";
	std::cout << "<< Search Information >> " << "\n";
	std::cout << "\x1b[33m" <<  "(TxID) :: ";
	for( int i=0; i<32; i++ )
	{
		printf("%02X", txID.get()[i]);
	} std::cout << "\n";
	std::cout << "(Index) :: " << index << "\n";
	std::cout << "(Key) :: ";
	for( auto itr : keyVector )
	{
		printf("%02X", static_cast<unsigned char>(itr) );
	} std::cout << "\n";
	std::cout << "-----------------------------------------------------------" << "\x1b[39m" << "\n";

	// json形式でクエリを作成
	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_GET;
	queryJson["key"] = nlohmann::json::binary( keyVector );

	// json形式のクエリを書き出す
	std::vector<uint8_t> dumpedQuery;
	dumpedQuery = nlohmann::json::to_bson( queryJson );


	// 書き出したクエリの型変換
	std::shared_ptr<unsigned char> dumpedRawQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQuery.size()] );
	std::copy( dumpedQuery.begin() , dumpedQuery.begin() + dumpedQuery.size() , dumpedRawQuery.get() );

	// SBに詰める
	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery, dumpedQuery.size() );

	// クエリを流す
	_pushSBContainer->pushOne( std::move(querySB) );


	// SBの監視中に別のレスポンスが入ってくる可能性がある -> 簡易的に取り出してIDが異なったら最後尾に追加する？


	/* クエリレスポンスの取り出し */
	re: // かなり雑に実装している必ず修正する必要がある
	std::unique_ptr<SBSegment> response = _popSBContainer->popOne();

	std::vector<uint8_t> responseVector;
	responseVector.assign( response->body().get() , response->body().get() + response->bodyLength() ); // レスポンスの型変換


	nlohmann::json responseJson = nlohmann::json::from_bson( responseVector );
	if( !(responseJson.contains("QueryID")) ) return nullptr;
	if( responseJson["QueryID"] != queryJson["QueryID"] )
	{
		_popSBContainer->pushOne( std::move(response) );
		goto re;
	}

	if( !(responseJson.contains("status")) ) return nullptr;
	if( (responseJson["status"]) != 0 ) return nullptr;

	// bsomにダンプしているデータのデコード
	nlohmann::json valueJson = nlohmann::json::from_bson( responseJson["value"].get_binary() );
	std::shared_ptr<UTXO> retUTXO = std::make_shared<UTXO>();
	if( retUTXO->importFromJson( valueJson ) )
		return retUTXO;

	return nullptr;
}




std::shared_ptr<UTXO> LightUTXOSet::get( std::shared_ptr<tx::prev_out> prevOut )
{
	return get( prevOut->txID() , prevOut->index() );
}


// MiyaDBClientに換装する
bool LightUTXOSet::add( std::shared_ptr<tx::TxOut> targetTxOut, std::shared_ptr<unsigned char> txID, uint32_t index )
{  // 単一のtxOutを登録する

	std::shared_ptr<UTXO> valueUTXO = std::make_shared<UTXO>( targetTxOut , txID ,index ); // 登録するUTXOの作成
	std::vector<uint8_t> dumpedUTXO = valueUTXO->dumpToBson();

	size_t joinedIDIndexLength = 32 + sizeof(index); // あとで置き換える
	std::shared_ptr<unsigned char> joinedIDIndex = std::shared_ptr<unsigned char>( new unsigned char[joinedIDIndexLength] );
	memcpy( joinedIDIndex.get() , txID.get() , 32 );
	memcpy( joinedIDIndex.get() + 32  , &index , sizeof(index) );

	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKeyLength = hash::SHAHash( joinedIDIndex, joinedIDIndexLength , &rawKey , "sha1" );

	std::vector<uint8_t> keyVector; keyVector.assign( rawKey.get() , rawKey.get() + rawKeyLength );

	std::cout << "\x1b[32m" << "-----------------------------------------------------------" << "\x1b[39m" << "\n";
	std::cout << " << Store Information >> " << "\n";
	std::cout << "\x1b[32m" << "[TxID] :: " ;
	for( int i=0; i<32; i++ )
	{
		printf("%02X", txID.get()[i]);
	} std::cout << "\n";
	std::cout << "[Index] :: " << index << "\n";
	std::cout << "[Key] :: ";
	for( auto itr : keyVector )
	{
		printf("%02X", static_cast<unsigned char>(itr));
	} std::cout << "\n";
	std::cout <<  "-----------------------------------------------------------" << "\x1b[39m" << "\n";


	nlohmann::json queryJson;
	queryJson["QueryID"] = miya_db::generateQueryID();
	queryJson["query"] = miya_db::MIYA_DB_QUERY_ADD;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["value"] = nlohmann::json::binary( dumpedUTXO );


	std::vector<uint8_t> dumpedQueryVector;
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );


	std::shared_ptr<unsigned char> dumpedQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin() , dumpedQueryVector.begin() + dumpedQueryVector.size() , dumpedQuery.get() );

	std::unique_ptr<SBSegment> query = std::make_unique<SBSegment>();
	query->body( dumpedQuery , dumpedQueryVector.size() );

	_pushSBContainer->pushOne( std::move(query) );

	std::unique_ptr<SBSegment> responseSB;
	std::vector<uint8_t> responseVector;
	nlohmann::json responseJson;
	for(;;){
		responseSB = _popSBContainer->popOne();
		responseVector.clear(); responseJson.clear();
		responseVector.assign( responseSB->body().get() , responseSB->body().get() + responseSB->bodyLength() );
		responseJson = nlohmann::json::from_bson( responseVector );

		if( responseJson.contains("QueryID") &&  (responseJson["QueryID"] == queryJson["QueryID"]) ) break;
		_popSBContainer->pushOne( std::move(responseSB) );
	}

	if( !(responseJson.contains("status")) ) return false;
	return (responseJson["status"] == 0 );
}



bool LightUTXOSet::add( std::shared_ptr<UTXO> target )
{
	std::cout << "< 1 >" << "\n";
	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKey = generateUTxOKey( target->txID() , target->outputIndex() );
	std::cout << "< 2 >" << "\n";

	std::cout << "< 3 >" << "\n";
	std::shared_ptr<unsigned char> rawUTxO; size_t rawUTxOLength;
	rawUTxOLength = target->dumpToBson( &rawUTxO );
	std::cout << "< 4 >" << "\n";

	return _miyaDBClient->add( rawKey ,rawUTxO , rawUTxOLength );
}





bool LightUTXOSet::add( std::shared_ptr<tx::coinbase> targetCoinbase )
{
	return false;
}


bool LightUTXOSet::remove( std::shared_ptr<unsigned char> txID, uint32_t index )
{
	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength = 20; // あとで修正する
	rawKey = generateUTxOKey( txID , index );

	return _miyaDBClient->remove( rawKey );
}






void LightUTXOSet::testInquire( std::shared_ptr<unsigned char> data , size_t dataLength )
{

	sleep(1);

	//std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();
	std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();


	std::shared_ptr<unsigned char> queryKey = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( queryKey.get() , "aaaaaaaaaaaaaaaaaaaa" , 20 );
	std::vector<uint8_t> keyVector; keyVector.assign( queryKey.get() , queryKey.get() + 20 );

	std::vector<uint8_t> valueVector; valueVector.assign( data.get() , data.get() + dataLength );


	nlohmann::json queryJson;
	queryJson["query"] = 1;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["value"] = nlohmann::json::binary( valueVector );


	std::vector<uint8_t> exportedJsonVector;
	exportedJsonVector = nlohmann::json::to_bson( queryJson );


	std::cout << queryJson << "\n";

	std::cout << "@@@ Value Length :::: " << valueVector.size() << "\n";

	std::shared_ptr<unsigned char> exportedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedJsonVector.size()] );
	std::copy( exportedJsonVector.begin() , exportedJsonVector.begin() + exportedJsonVector.size() , exportedJsonRaw.get() );

	segment->body( exportedJsonRaw , exportedJsonVector.size() );

	_pushSBContainer->pushOne( std::move(segment) );



	sleep(1);

	std::unique_ptr<SBSegment> responseSegment = std::make_unique<SBSegment>();

	responseSegment = _popSBContainer->popOne();


	std::cout << "データの登録OK" << "\n";




	std::cout << "\n\n\n\n-------------------------------------" << "\n";

	std::unique_ptr<SBSegment> requestSegment = std::make_unique<SBSegment>();

	nlohmann::json requestJson;
	requestJson["query"] = 2;// get
	requestJson["key"] = nlohmann::json::binary( keyVector );

	std::vector<uint8_t> exportedRequestJsonVector = nlohmann::json::to_bson( requestJson );
	std::shared_ptr<unsigned char> exportedRequestJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedRequestJsonVector.size()] );
	std::copy( exportedRequestJsonVector.begin(), exportedRequestJsonVector.begin() + exportedRequestJsonVector.size() , exportedRequestJsonRaw.get() );

	requestSegment->body( exportedRequestJsonRaw, exportedRequestJsonVector.size() );
	_pushSBContainer->pushOne( std::move(requestSegment) );


	std::unique_ptr<SBSegment> getResponseSegment = _popSBContainer->popOne();
	std::cout << "とりあえず仮取得できました" << "\n";

	std::cout << "----------- 取得したデータ -----------" << "\n";
	std::vector<uint8_t> getResponseSegmentVector;
	getResponseSegmentVector.assign( getResponseSegment->body().get() , getResponseSegment->body().get() + getResponseSegment->bodyLength() );

	nlohmann::json ret = nlohmann::json::from_bson( getResponseSegmentVector );

	std::cout << ret << "\n";
	std::cout << getResponseSegment->bodyLength() << "\n";


	return;
}






}
