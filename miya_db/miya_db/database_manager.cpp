#include "database_manager.h"

#include "../../shared_components/json.hpp"
#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"

#include "./query_context/query_context.h"

#include "../strage_manager/MMyISAM/MMyISAM.h"
#include "../strage_manager/MMyISAM/components/index_manager/index_manager.h"
#include "../strage_manager/MMyISAM/components/index_manager/btree.h"


namespace miya_db{






void DatabaseManager::loadInformationSchema( char *path )
{
	FILE *schema = nullptr;

	//fopen( path , "");
}




void DatabaseManager::startQueryHandleThread( bool isAdditionalThread )
{
	/*
	std::thread queryHanalder( [&](){
		std::cout << "query handler started" << "\n";

		unsigned char* segment;  unsigned int segmentSize;

		int availableSpace = 0;

		if( isAdditionalThread ){
			for( int i=0; i<3; i++) // 3回ブロッキングなしでバッファをチェックしていずれも空だったら,そのスレッドは破棄する
			{ 
			 	availableSpace =	_inbandMiddleBuffer->popOne() &segment , &segmentSize , false ;
				if( avaiableSpace > 0 ) break;
				else continue;

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			if( avaiableSpace <= 0 ) return; // ３回バッファに問い合わせても空だったらリターンする

		}else{
			availableSpace = _inbandMiddleBuffer->popOne( &segment , &segmentSize , true );
		}

		if( (( avaiableSpace / _inbandMiddleBuffer->_bufferSize )*100)  >= 30 ){ // バッファ使用率が任意の値を超えたら
			_startQueryHandleThread( true ); // 追加ハンドラの起動
		}

			
		QueryPack *queryPack = new QueryPack;
		queryPack->importRaw( segment , segmentSize );
		delete segment;


		
		delete queryPack;
	});
	*/
}


void DatabaseManager::hello()
{
	
	std::cout << "Hello" << "\n";
}


void DatabaseManager::startWithLightMode( std::shared_ptr<StreamBufferContainer> popSBContainer, std::shared_ptr<StreamBufferContainer> pushSBContainer ,std::string fileName )
{
	std::cout << "Launching MiyaDB [ Light Mode ]" << "\n";
	std::shared_ptr<MMyISAM> mmyisam = std::shared_ptr<MMyISAM>( new MMyISAM(fileName) ); // 簡易的に指定のストレージエンジンを使用


	std::cout << "MMyISAM setuped" << "\n";

	printf(" mmyisam -> %p\n", mmyisam.get() );
	printf(" mmyisam->indexManager -> %p\n", mmyisam->_indexManager.get() );
	printf(" mmyisam->indexManager->masterBtree -> %p\n", mmyisam->_indexManager->_masterBtree.get() );

	std::shared_ptr<int> num = std::make_shared<int>(10);
	printf(" num -> %d\n", *num );
	printf(" num -> %p\n", num.get() );

	// respondスレッドを用意する &(参照)でキャプチャするとスマートポインタのアドレスが変わる
	std::thread lightMiyaDBThread([mmyisam, popSBContainer, pushSBContainer, this]() 
	{
		std::cout << "Started MiayDB(light) Handler Thread" << "\n";
		

		std::unique_ptr<SBSegment> sbSegment;
		std::vector<uint8_t> dumpedJson;
		std::shared_ptr<unsigned char> dumpedJsonRaw; 
		nlohmann::json responseJson; responseJson["status"] = -1;
		int flag;
		for(;;)
		{
			std::cout << "< check 0 >" << "\n";

			printf( "%p\n", popSBContainer->_sbs.at(0).get() );


			// 1. ポップ
			sbSegment = popSBContainer->popOne();


			std::cout << "< check 1 >" << "\n";

			// クエリの取り出し
			//  クエリの解析と対応する操作メソッドの呼び出し
			std::shared_ptr<QueryContext> qctx;
			qctx = parseQuery( sbSegment->body() , sbSegment->bodyLength() );

			std::cout << "< check 2 >" << "\n";
			printf("pointer -> %p\n", qctx.get() );

			std::cout << "type() -> " << qctx->type() << "\n";
			

			// 2. 処理
			switch( qctx->type() )
			{
				case QUERY_ADD: // 1 add
					std::cout << "## (HANDLE) QUERY_ADD" << "\n";
					flag = mmyisam->add( qctx );

					std::cout << "Flag -> " << flag << "\n";
					responseJson["status"] = 0;

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() , dumpedJsonRaw.get() );
					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );
					break;
				
				case QUERY_SELECT: // 2 get
					std::cout << "## (HANDLE) QUERY_SELECT"	<< "\n";
					std::cout << "< check 2.5 >" << "\n";

					printf("%p\n", mmyisam.get() );
					printf("%p\n", qctx.get() );
					puts("mmyisam->get() call before");

					mmyisam->hello();
					flag = mmyisam->get( qctx );

					std::cout << "< check 3 >" << "\n";

					responseJson["status"] = 0;
					std::vector<uint8_t> valueVector; valueVector.assign( qctx->value().get(), qctx->value().get() + qctx->valueLength() );
					responseJson["value"] = nlohmann::json::binary( valueVector );

					std::cout << "< check 4 >" << "\n";

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() ,  dumpedJsonRaw.get() );

					std::cout << "< check 5 >" << "\n";

					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );

					std::cout << "< check 6 >" << "\n";
					break;
			}
		
			pushSBContainer->pushOne( std::move(sbSegment) );
		}
		
		//
	});

	lightMiyaDBThread.detach();

}





}; // close miya_db namespace

