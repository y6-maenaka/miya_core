#ifndef B024B40B_546B_4907_B464_A22438B617CD
#define B024B40B_546B_4907_B464_A22438B617CD

#include <thread>
#include <chrono>
#include <memory>
#include <vector>

#include <unistd.h>



#include "./query_parser/query_parser.h"


class StreamBuffer;
class StreamBufferContainer;


class InformationSchema;





namespace miya_db{

class ConnectionManager;





constexpr char *INFORMATION_SCHEMA_PATH = "./.config/.information_schema.json";

constexpr int QUERY_ADD = 1;
constexpr int QUERY_SELECT = 2;
constexpr int QUERY_EXISTS = 3;
constexpr int QUERY_REMOVE = 4;

constexpr int QUERY_MIGRATE_SAFE_MODE = 10;
constexpr int QUERY_SAFE_MODE_COMMIT = 11;
constexpr int QUERY_SAFE_MODE_ABORT = 12;



class DatabaseManager : public QueryParser
	{

private:

	std::shared_ptr<InformationSchema> _informationSchema; // plannerが参照することが多い
																				 // ここにテーブルがロードされているか否かなども保存される


protected:
	void loadInformationSchema( char *path ); // config.json ファイルよりデータベーススキーマを取得してくる

public:
	/* 受信バッファからデータを取り出し, parser, planner, executre を経てレスポンスを返す */
	/* 基本的に単体で使用されるが, バッファ内の状況に応じては複数起動される、　最低でも一つのスレッドは起動している　*/
	void startQueryHandleThread( bool isAdditionalThread = false ); // 基本的にこのスレッドがクエリを解析し,データを取得し,レスポンスを作成する. // additional=trueであれば,キャパが余れば自身で勝手に解放する
	void init();

	void hello();

	// 本来はstreamBuffer内のbufferではなく生のバイナリフォーマットbsonでメッセージ交換する
	void startWithLightMode( std::shared_ptr<StreamBufferContainer> incomingSBC , std::shared_ptr<StreamBufferContainer> outgoingSBC ,std::string dbName ); // 簡易的に起動する場合
	/* パイプの役割はStreamBufferで代替する */
};


}; // close miya_db namespace


#endif // B024B40B_546B_4907_B464_A22438B617CD
