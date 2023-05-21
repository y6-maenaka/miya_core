#ifndef B024B40B_546B_4907_B464_A22438B617CD
#define B024B40B_546B_4907_B464_A22438B617CD




namespace miya_db{

class ConnectionManager;







class DatabaseManager{

private:
	
	struct ConnectionInterface
	{
		ConnectionManager *_connectionManager;
		MiddleBuffer *_middlerBuffer;

		ConnectionInterface( MiddleBuffer *middleBuffer ){
			// set mifflebuffer ptr to connection manager under module
			_connectionManager = ConnectionManager( middleBuffer );
		}
		
	}	_connectionInterface;

public:
	/* 受信バッファからデータを取り出し, parser, planner, executre を経てレスポンスを返す */
	/* 基本的に単体で使用されるが, バッファ内の状況に応じては複数起動される、　最低でも一つのスレッドは起動している　*/
	void startQueryHandleThread(); // 基本的にこのスレッドがクエリを解析し,データを取得し,レスポンスを作成する.

};


}; // close miya_db namespace


#endif // B024B40B_546B_4907_B464_A22438B617CD

