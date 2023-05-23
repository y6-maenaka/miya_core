#ifndef D32265E0_ABAC_4334_9724_9BB4117CD27B
#define D32265E0_ABAC_4334_9724_9BB4117CD27B


#include <iostream>
#include <vector>

#include <sys/select.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/socket.h>



namespace miya_db{

constexpr long DEFAULT_INTERVAL_TIME = 1000;
constexpr long DEFAULT_MAX_PENDING = 5;
constexpr char *MIYA_DB_ID = 0x0056;




struct MiyaDBMSG
{
	struct header
	{
		uint16_t _msgLen; // network byte order
		unsigned char _sessionid[4];
		unsigned char _id[2];
		unsigned char _free[4];

	} _header;

	unsigned char* _payload;
	
	MiyaDBMSG( unsigned int msgLen );
	~MiyaDBMSG();
	bool validate();
};




struct QueryPack // for middlebuffer between databseManager & connectionManager
{
	struct IExportTarget
	{
		int _clientSock;
		unsigned char *_query;

	} _iexportTarget;

	unsigned int _querySize;


	QueryPack( int clientSock , unsigned char* query , unsigned int querySize ) // セット
	{
		_iexportTarget->_clientSock = clientSock;

		_iexportTarget->_query = new unsigned char[querySize];
		_iexportTarget->_query = query;
		_querySize = querySize;
	};


	unsigned int exportRaw( unsigned char **ret );
	unsigned int importRaw( unsigned char *from , unsigned int fromSize );
};






class ConnectionManager
{

private:
	int _connectedSock;

	MiddleBuffer *_inbandMiddleBuffer;

	struct fds
	{
		fd_set _readFDS;
		int _nfds;
	} _fds;



protected:
	void initFD();
	bool noticeMidBuffPtr( DataBaseManager::setMidBuffPtr_func ); // ミスしたらどうする

public:
	// コネクションと通信を監視するメソッドの実装

	void init();
	// 中間バッファの確保とcallbackによる上位モジュールへの中間バッファポインタの通知

	// acceptだけは別のスレッドで動かす？	
	
	void startInbandMonitor( int listenSoc ); // acceptとrecv両方を行う
	void response( int clientSock , unsigned char *target , unsigned int targetSize );

	// ConnectionManager();
};





}; // close miya_db namespace

#endif // D32265E0_ABAC_4334_9724_9BB4117CD27B

