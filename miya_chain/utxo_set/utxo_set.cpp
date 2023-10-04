#include "utxo_set.h"


#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"
#include "../../shared_components/json.hpp"




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





void LightUTXOSet::testInquire()
{
	sleep(1);

	//std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();
	std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();

	std::cout << "check 3" << "\n";

	std::shared_ptr<unsigned char> queryKey = std::shared_ptr<unsigned char>( new unsigned char[20] );
	memcpy( queryKey.get() , "aaaaaaaaaaaaaaaaaaaa" , 20 );
	std::vector<uint8_t> keyVector; keyVector.assign( queryKey.get() , queryKey.get() + 20 );

	std::cout << "check 4" << "\n";

	nlohmann::json queryJson;
	queryJson["query"] = 2;
	queryJson["key"] = nlohmann::json::binary( keyVector );

	std::cout << "check 5" << "\n";


	std::vector<uint8_t> exportedJsonVector;
	exportedJsonVector = nlohmann::json::to_bson( queryJson );

	std::cout << "check 6" << "\n";

	std::shared_ptr<unsigned char> exportedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[exportedJsonVector.size()] );
	std::copy( exportedJsonVector.begin() , exportedJsonVector.begin() + exportedJsonVector.size() , exportedJsonRaw.get() );

	std::cout << "check 7" << "\n";

	segment->body( exportedJsonRaw , exportedJsonVector.size() );

	std::cout << "check 8" << "\n";

	_pushSBContainer->pushOne( std::move(segment) );

	std::cout << "check 9" << "\n";






	sleep(2);
	std::cout << "-------------------------------------" << "\n";

	std::unique_ptr<SBSegment> responseSegment = std::make_unique<SBSegment>();

	responseSegment = _popSBContainer->popOne();

	std::cout << "poped" << "\n";



	return;
}






}
