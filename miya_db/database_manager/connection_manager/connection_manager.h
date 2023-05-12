#ifndef D32265E0_ABAC_4334_9724_9BB4117CD27B
#define D32265E0_ABAC_4334_9724_9BB4117CD27B


#include <iostream>
#include <vector>

#include <sys/select.h>
#include <stdlib.h>


namespace miya_db{

constexpr long DEFAULT_INTERVAL_TIME = 1000;










class Client // 将来的に認証機能を追加するかもしれないため生のSocketでは扱わない
{

private:
	TCPSocketManager _sockManager;
	// 認証情報


public:


};









class ConnectionManager
{

private:
	std::pair< int , std::vectot<TCPSocketManager> > _client;
	          // handlerClientCnt  ,  clients

	MidBuffer *_midBiffer;

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
	
	void monitor( int listenSoc ); // acceptとrecv両方を行う

	ConnectionManager();
};





}; // close miya_db namespace

#endif // D32265E0_ABAC_4334_9724_9BB4117CD27B

