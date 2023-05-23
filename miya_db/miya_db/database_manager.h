#ifndef B024B40B_546B_4907_B464_A22438B617CD
#define B024B40B_546B_4907_B464_A22438B617CD

#include <thread>
#include <chrono>


namespace miya_db{

class ConnectionManager;


constexpr char *INFORMATION_SCHEMA_PATH = "./.config/.information_schema.json"

class DatabaseManager{

private:
	
	struct ConnectionInterface
	{
		ConnectionManager *_connectionManager;
		MiddleBuffer *_inbandMiddlerBuffer;

		ConnectionInterface( MiddleBuffer *middleBuffer ){ // constructor
			// set mifflebuffer ptr to connection manager under module
			_connectionManager = ConnectionManager( middleBuffer );
		}
		
	}	_connectionInterface;


	InformationSchema *_informationSchema; // plannerが参照することが多い
																				 // ここにテーブルがロードされているか否かなども保存される



protected:
	void loadInformationSchema( char *path ); // config.json ファイルよりデータベーススキーマを取得してくる

public:
	/* 受信バッファからデータを取り出し, parser, planner, executre を経てレスポンスを返す */
	/* 基本的に単体で使用されるが, バッファ内の状況に応じては複数起動される、　最低でも一つのスレッドは起動している　*/
	void startQueryHandleThread( bool isAdditionalThread = false ); // 基本的にこのスレッドがクエリを解析し,データを取得し,レスポンスを作成する. // additional=trueであれば,キャパが余れば自身で勝手に解放する
	void init();

};


}; // close miya_db namespace


#endif // B024B40B_546B_4907_B464_A22438B617CD

