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


void DatabaseManager::startWithLightMode( std::shared_ptr<StreamBufferContainer> incomingSBC, std::shared_ptr<StreamBufferContainer> outgoingSBC ,std::string filePath )
{
	std::cout << "Launching MiyaDB [ Light Mode ]" << "\n";
	std::shared_ptr<MMyISAM> mmyisam = std::shared_ptr<MMyISAM>( new MMyISAM(filePath) ); // 簡易的に指定のストレージエンジンを使用


	// respondスレッドを用意する &(参照)でキャプチャするとスマートポインタのアドレスが変わる ※ 呼び出し元スレッドが死んでいる
	std::thread lightMiyaDBThread([mmyisam, incomingSBC, outgoingSBC, this]() 
	{
		std::unique_ptr<SBSegment> sbSegment;
		std::vector<uint8_t> dumpedJson;
		std::shared_ptr<unsigned char> dumpedJsonRaw; 
		
		int flag;
		std::shared_ptr<QueryContext> qctx;



		auto failureSB = []( std::shared_ptr<QueryContext> qctx ) -> std::unique_ptr<SBSegment>
		{
			std::cout << "Sorry. FailureSB Generated" << "\n";

			std::unique_ptr<SBSegment> ret = std::make_unique<SBSegment>();
			nlohmann::json failureMSG; 
			failureMSG["QueryID"] = qctx->id();
			failureMSG["status"] = -1;

			std::vector<uint8_t> failureMSGVector = nlohmann::json::to_bson( failureMSG );
			std::shared_ptr<unsigned char> failureMSGRaw = std::shared_ptr<unsigned char>( new unsigned char[failureMSGVector.size()] );

			std::copy( failureMSGVector.begin() , failureMSGVector.begin() + failureMSGVector.size() , failureMSGRaw.get() );
			ret->body( failureMSGRaw , failureMSGVector.size() );

			return std::move(ret);
		};


		nlohmann::json responseJson;
		for(;;)
		{
			responseJson.clear();
			// 1. ポップ
			sbSegment = incomingSBC->popOne();
			std::cout << "(MiyaDB) Query Poped" << "\n";
			printf("%p\n", sbSegment.get() );
			printf("%p - %d\n", sbSegment->body().get(), sbSegment->bodyLength() );

			// クエリの取り出し
			//  クエリの解析と対応する操作メソッドの呼び出し
			qctx = parseQuery( sbSegment->body() , sbSegment->bodyLength() ); // ここで定義するのはオーバーフローが大きい

			std::cout << "(MiyaDB) Query Parsed" << "\n";

			if( qctx == nullptr ) { // クエリメッセージの解析に失敗した場合
				sbSegment = failureSB( qctx );
				std::cout << "(MiyaDB) Handle query failure" << "\n";
				goto direct;
			}

			std::cout << "(MiyaDB) QueryType :: " << qctx->type() << "\n";

			// 2. 処理
			switch( qctx->type() )
			{



				case QUERY_ADD: // 1 add
				{
					std::cout << "\x1b[32m" << "## (HANDLE) QUERY_ADD" << "\x1b[39m" << "\n";

					flag = mmyisam->add( qctx ); // addの実行
					if( !flag ){
						sbSegment = failureSB( qctx );
						goto direct;
					}
					// Meta
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = 0;

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() , dumpedJsonRaw.get() );
					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );
					break;
				}
			




				case QUERY_SELECT: // 2 get
				{
					std::cout << "\x1b[34m" << "## (HANDLE) QUERY_SELECT"	<<"\x1b[39m" << "\n";

					flag = mmyisam->get( qctx );
					if( !flag ){
						sbSegment = failureSB( qctx );
						goto direct;
					}
					// Meta
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = 0;

					std::vector<uint8_t> valueVector; valueVector.assign( qctx->value().get(), qctx->value().get() + qctx->valueLength() );
					responseJson["value"] = nlohmann::json::binary( valueVector );

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() ,  dumpedJsonRaw.get() );
					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );
					
					break;
				}


				case QUERY_EXISTS: // 3 exists
				{
					std::cout <<  "## (HANDLE) QUERY_EXISTS"	<< "\n";

					flag = mmyisam->exists( qctx );

					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = 0; // 成功
					responseJson["value"] = flag;

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() , dumpedJsonRaw.get() );
					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );
					break;
				}
			}
	
			direct:
			outgoingSBC->pushOne( std::move(sbSegment) );
			
		}
		
		//
	});

	lightMiyaDBThread.detach();
	std::cout << "LightMiyaDBThread Detached" << "\n";

}





}; // close miya_db namespace

