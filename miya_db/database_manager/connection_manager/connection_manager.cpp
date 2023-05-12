#include "connection_manager.h"


namespace miya_db{












void ConnectionManager::monitor( int listenSock )
{

	 int activeCnt;
		struct timeval tv;
		tv.tv_sec = 5;   tv.tv_usec = 0;


		FD_ZERO( &_fds._readFDS ); // クリア
		// accept対象のlistenSockをFDへ追加
		FD_SET( listenSock , &_fds._readFDS );
		_fds._nfds = listenSock; // スタート時はリッスンしているソケットしかないため

		for(;;){



			if( (activeCnt = select( _fds._nfds + 1 , _fds._ndfs , nullptr , nullptr , &tv )) < -1 ){
				if( errno == EBADF ){
					// 無効になっているソケットディスクリプタを探し出す
				}
			}


			for( int i=0; i < activeCnt ; i++ ){
				// 更新があったソケットディスクリプタの個数処理を回す
				for(){
					// 変更があったファイルディスクリプタを探し出す 
				}
			}

			recvBuff = socket.recv();
	

	}

}







ConnectionManager::ConnectionManager()
{
	
}









}; // close miya_db namespace
