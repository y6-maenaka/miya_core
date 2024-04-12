#include "database_manager.h"

#include "../../share/json.hpp"
#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"

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


void DatabaseManager::hello()
{

	std::cout << "Hello" << "\n";
}


void DatabaseManager::startWithLightMode( std::shared_ptr<StreamBufferContainer> incomingSBC, std::shared_ptr<StreamBufferContainer> outgoingSBC ,std::string dbName )
{
	std::cout << "Launching MiyaDB [ Light Mode ]" << "\n";
	std::shared_ptr<MMyISAM> mmyisam = std::shared_ptr<MMyISAM>( new MMyISAM(dbName) ); // 簡易的に指定のストレージエンジンを使用


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


		auto dumpToSBBody = [&]( nlohmann::json from, std::shared_ptr<unsigned char> *retRaw ) -> size_t
		{
		  nlohmann::json dumpedJson = nlohmann::json::to_bson(from);
		  size_t retLength = dumpedJson.size();
		  *retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
		  std::copy( dumpedJson.begin(), dumpedJson.end(), (*retRaw).get() );
		  return retLength;
		};


		nlohmann::json responseJson;
		for(;;)
		{
			responseJson.clear();
			// 1. ポップ
			sbSegment = incomingSBC->popOne();
			std::cout << "(MiyaDB) Query Poped" << "\n";

			// クエリの取り出し
			//  クエリの解析と対応する操作メソッドの呼び出し
			qctx = parseQuery( sbSegment->body() , sbSegment->bodyLength() ); // ここで定義するのはオーバーフローが大きい

			std::cout << "(MiyaDB) Query Parsed" << "\n";

			if( qctx == nullptr || qctx->type() < 0 ) { // クエリメッセージの解析に失敗した場合
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
					responseJson["status"] = flag;

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
						std::cout << "\x1b[31m" << "データセットが見つかりませんでした" << "\x1b[39m" << "\n";
						goto direct;
					}
					// Meta
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = flag;

					std::cout << "(MiyaDB) qctx->valueLength() :: " << qctx->valueLength() << "\n";

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
					responseJson["status"] = flag; // 成功
					responseJson["value"] = flag;

					dumpedJson = nlohmann::json::to_bson( responseJson );
					dumpedJsonRaw = std::shared_ptr<unsigned char>( new unsigned char[dumpedJson.size()] );
					std::copy( dumpedJson.begin() , dumpedJson.begin() + dumpedJson.size() , dumpedJsonRaw.get() );
					sbSegment->body( dumpedJsonRaw , dumpedJson.size() );
					break;
				}


				case QUERY_REMOVE:
				{
					std::cout << "## (HANDLE) QUERY_REMOVE" << "\n";

					flag = mmyisam->remove( qctx );
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = flag;

					std::shared_ptr<unsigned char>	 dumpedJsonRaw; size_t dumpedJsonRawLength;
					dumpedJsonRawLength = dumpToSBBody( responseJson , &dumpedJsonRaw );
					sbSegment->body( dumpedJsonRaw , dumpedJsonRawLength );
					break;
				}


				case QUERY_MIGRATE_SAFE_MODE:
				{
					std::cout << "## (HANDLE) QUERY_SWITCH_TO_SAFE_MODE" << "\n";

					flag = mmyisam->migrateToSafeMode( qctx );
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = flag;

					// レスポンスSBのセットアップ
					std::shared_ptr<unsigned char> dumpedJsonRaw; size_t dumpedJsonRawLength;
					dumpedJsonRawLength = dumpToSBBody( responseJson , &dumpedJsonRaw );
					sbSegment->body( dumpedJsonRaw , dumpedJsonRawLength );
					break;
				}

				case QUERY_SAFE_MODE_COMMIT:
				{
					std::cout << "## (HANDLE) QUERY_SAFE_MODE_COMMIT" << "\n";

					flag = mmyisam->safeCommitExit(qctx);
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = flag;

					// レスポンスSBのセットアップ
					std::shared_ptr<unsigned char> dumpedJsonRaw; size_t dumpedJsonRawLength;
					dumpedJsonRawLength = dumpToSBBody( responseJson , &dumpedJsonRaw );
					sbSegment->body( dumpedJsonRaw , dumpedJsonRawLength );
					break;
				}

				case QUERY_SAFE_MODE_ABORT:
				{
					std::cout << "## (HANDLE) QUERY_SAFE_MODE_ABORT" << "\n";

					flag = mmyisam->safeAbortExit(qctx);
					responseJson["QueryID"] = qctx->id();
					responseJson["status"] = flag;

					// レスポンスSBのセットアップ
					std::shared_ptr<unsigned char> dumpedJsonRaw; size_t dumpedJsonRawLength;
					dumpedJsonRawLength = dumpToSBBody( responseJson , &dumpedJsonRaw );
					sbSegment->body( dumpedJsonRaw , dumpedJsonRawLength );
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
