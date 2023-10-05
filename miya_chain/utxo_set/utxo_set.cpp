#include "utxo_set.h"


#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/json.hpp"

#include "../transaction/p2pkh/p2pkh.h"


namespace miya_chain
{



LightUTXOSet::LightUTXOSet( std::shared_ptr<StreamBufferContainer> pushSBContainer , std::shared_ptr<StreamBufferContainer> popSBContainer )
{
	_pushSBContainer = pushSBContainer;
	_popSBContainer = popSBContainer;
}




std::shared_ptr<UTXO> LightUTXOSet::find( std::shared_ptr<unsigned char> txID , unsigned short index )
{
	return nullptr;
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



	sleep(2);

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
