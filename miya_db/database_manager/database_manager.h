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


};


}; // close miya_db namespace


#endif // B024B40B_546B_4907_B464_A22438B617CD

