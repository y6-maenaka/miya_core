#include "utxo_set.h"


#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/json.hpp"
#include "../../shared_components/hash/sha_hash.h"

#include "../transaction/p2pkh/p2pkh.h"
#include "../transaction/tx/tx_out.h"

#include "./UTXO.h"


namespace miya_chain
{



LightUTXOSet::LightUTXOSet( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer )
{
	_pushSBContainer = pushSBContainer;
	_popSBContainer = popSBContainer;
}





uint32_t LightUTXOSet::generateQueryID()
{
	std::random_device rd; // シードの初期化
	std::mt19937 gen(rd());  // 乱数生成機の初期化

	std::uniform_int_distribution<uint32_t> dis; // 生成範囲の設定
	return dis(gen);
}




std::shared_ptr<UTXO> LightUTXOSet::get( std::shared_ptr<unsigned char> txID , uint32_t index )
{

	std::cout << "( 1 )" << "\n";

	//  キーを作成する
	size_t joinedIDIndexLength = 32 + sizeof(index);
	std::shared_ptr<unsigned char> joinedIDIndex = std::shared_ptr<unsigned char>( new unsigned char[joinedIDIndexLength] );
	memcpy( joinedIDIndex.get() , txID.get() , 32 );
	memcpy( joinedIDIndex.get() + 32 , &index , sizeof(index) );


	std::cout << "( 2 )" << "\n";

	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKeyLength = hash::SHAHash( joinedIDIndex , joinedIDIndexLength, &rawKey , "sha1" );

	std::vector<uint8_t> keyVector; keyVector.assign( rawKey.get() , rawKey.get() + rawKeyLength );

	std::cout << "( 3 )" << "\n";

	// json形式でクエリを作成
	nlohmann::json queryJson;
	queryJson["QueryID"] = LightUTXOSet::generateQueryID();
	queryJson["query"] = MIYA_DB_QUERY_GET;
	queryJson["key"] = nlohmann::json::binary( keyVector );


	std::cout << "( 4 )" << "\n";

	// json形式のクエリを書き出す
	std::vector<uint8_t> dumpedQuery;
	dumpedQuery = nlohmann::json::to_bson( queryJson );


	std::cout << "( 5 )" << "\n";


	// 書き出したクエリの型変換
	std::shared_ptr<unsigned char> dumpedRawQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQuery.size()] );
	std::copy( dumpedQuery.begin() , dumpedQuery.begin() + dumpedQuery.size() , dumpedRawQuery.get() );


	std::cout << "( 6 )" << "\n";
	std::cout << dumpedQuery.size() << "\n";

	sleep(1);

	// SBに詰める
	std::unique_ptr<SBSegment> querySB = std::make_unique<SBSegment>();
	querySB->body( dumpedRawQuery, dumpedQuery.size() );

	sleep(1);

	std::cout << "( 7 )" << "\n";

	// クエリを流す


	std::cout << "<1> >> " << _pushSBContainer->_sbs.at(0)->_sb._queue.size() << "\n";
	std::cout << "Query Pushed" << "\n";
	_pushSBContainer->pushOne( std::move(querySB) );
	std::cout << "<2> >> " << _pushSBContainer->_sbs.at(0)->_sb._queue.size() << "\n";

	
	// SBの監視中に別のレスポンスが入ってくる可能性がある -> 簡易的に取り出してIDが異なったら最後尾に追加する？


	/* クエリレスポンスの取り出し */
	re: // かなり雑に実装している必ず修正する必要がある
	std::unique_ptr<SBSegment> response = _popSBContainer->popOne();
	std::cout << "++++++++++++++++++++++++++" << "\n";
	std::cout << "Queue Size -> " << _popSBContainer->_sbs.at(0)->_sb._queue.size() << "\n";
	std::cout << "++++++++++++++++++++++++++" << "\n";
	sleep(1);


	std::cout << "( 8 )" << "\n";

	std::vector<uint8_t> responseVector;
	responseVector.assign( response->body().get() , response->body().get() + response->bodyLength() ); // レスポンスの型変換

	
	std::cout << "( 9 )" << "\n";


	nlohmann::json responseJson = nlohmann::json::from_bson( responseVector );
	std::cout << responseJson << "\n";

	if( !(responseJson.contains("QueryID")) ) return nullptr;
	if( responseJson["QueryID"] != queryJson["QueryID"] ) 
	{
		std::cout << "#### QueryID >>> " << responseJson["QueryID"] << "\n";
		std::cout << "@@@@ QueryID >>> " << queryJson["QueryID"] << "\n";
		_popSBContainer->pushOne( std::move(response) );
		goto re;
	}


	std::cout << "( 10 )" << "\n";

	if( !(responseJson.contains("status")) ) return nullptr;

	std::cout << "( 10.5 )" << "\n";
	std::cout << responseJson["status"] << "\n";

	if( (responseJson["status"]) != 0 ) return nullptr;


	std::cout << "( 11 )" << "\n";
	
	nlohmann::json valueJson = nlohmann::json::from_bson( responseJson["value"].get_binary() );
	std::shared_ptr<UTXO> retUTXO = std::make_shared<UTXO>();
	if( retUTXO->set( valueJson ) )
		return retUTXO;


	std::cout << "( 12 )" << "\n";


	return nullptr;
}




bool LightUTXOSet::store( std::shared_ptr<tx::TxOut> targetTxOut, std::shared_ptr<unsigned char> txID, uint32_t index )
{  // 単一のtxOutを登録する

	std::cout << "< check 1 >" << "\n";


	std::shared_ptr<UTXO> valueUTXO = std::make_shared<UTXO>( targetTxOut ); // 登録するUTXOの作成
	std::vector<uint8_t> dumpedUTXO = valueUTXO->dumpToBson();


	std::cout << "< check 2 >" << "\n";


	size_t joinedIDIndexLength = 32 + sizeof(index); // あとで置き換える
	std::shared_ptr<unsigned char> joinedIDIndex = std::shared_ptr<unsigned char>( new unsigned char[joinedIDIndexLength] );  
	memcpy( joinedIDIndex.get() , txID.get() , 32 );
	memcpy( joinedIDIndex.get() + 32  , &index , sizeof(index) );

	
	std::cout << "********************************" << "\n";
	for( int i=0; i<32; i++ )
	{
		printf("%02X", joinedIDIndex.get()[i]);
	} std::cout << "\n";
	std::cout << "********************************" << "\n";

	std::cout << "< check 3 >" << "\n";

	std::shared_ptr<unsigned char> rawKey; size_t rawKeyLength;
	rawKeyLength = hash::SHAHash( joinedIDIndex, joinedIDIndexLength , &rawKey , "sha1" );

	std::vector<uint8_t> keyVector; keyVector.assign( rawKey.get() , rawKey.get() + rawKeyLength );


	std::cout << "< check 4 >" << "\n";


	nlohmann::json queryJson;
	queryJson["QueryID"] = LightUTXOSet::generateQueryID();
	queryJson["query"] = MIYA_DB_QUERY_ADD;
	queryJson["key"] = nlohmann::json::binary( keyVector );
	queryJson["value"] = nlohmann::json::binary( dumpedUTXO );


	std::cout << "< check 5 >" << "\n";


	std::vector<uint8_t> dumpedQueryVector;
	dumpedQueryVector = nlohmann::json::to_bson( queryJson );


	std::cout << "< check 6 >" << "\n";
	std::cout << queryJson << "\n";

	std::shared_ptr<unsigned char> dumpedQuery = std::shared_ptr<unsigned char>( new unsigned char[dumpedQueryVector.size()] );
	std::copy( dumpedQueryVector.begin() , dumpedQueryVector.begin() + dumpedQueryVector.size() , dumpedQuery.get() );
	
	
	std::cout << "< check 7 >" << "\n";

	std::unique_ptr<SBSegment> query = std::make_unique<SBSegment>();
	query->body( dumpedQuery , dumpedQueryVector.size() );


	std::cout << "< check 8 >" << "\n";

	_pushSBContainer->pushOne( std::move(query) );



	std::cout << "-----------------------------------" << "\n"; // レスポンス受信

	
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




bool LightUTXOSet::store( std::shared_ptr<tx::P2PKH> targetTx )
{
	std::cout << "--- 1 ---" << "\n";

	std::shared_ptr<unsigned char> txID; size_t TxIDLength;
	TxIDLength = targetTx->calcTxID( &txID );

	std::cout << "--- 2 ---" << "\n";


	uint32_t index;
	for( size_t i =0; i < targetTx->outs().size(); i++ )
	{
		this->store( targetTx->outs().at(i) , txID, i );
	}


	std::cout << "--- 3 ---" << "\n";
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
