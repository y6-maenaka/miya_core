#include "connection_manager.h"


namespace miya_db{

int seupTCPSocket( unsigned short port );








unsigned int QueryPack::exportRaw( unsigned char **ret )
{
	unsigned int exportSize;

	*ret = new unsigned char[ _querySize + sizeof(_iexportTarget->_clientSock) ];
	//memcpy( *ret , _iexportTarget->_query , _querySize );
	memcpy( *ret , _iexportTarget->_clientSock , sizeof(_iexportTarget->_clientSock) );
	memcpy( *ret + sizeof(_iexportTarget->_clientSock) , _iexportTarget->_query , _querySize );

	return exportSize;
}



unsigned int QueryPack::importRaw( unsigned char *from , unsigned int fromSize )
{
	unsigned int querySize;
	querySize = fromSize - sizeof( _iexportTarget->_clientSock );

	memcpy( _iexportTarget->_clientSock , from , sizeof(_iexportTarget->_clientSock) ); // cpy client sock

	_iexportTarget->_query = new unsigned char[ fromSize - sizeof(_iexportTarget->_clientSock)];
	memcpy( _iexportTarget->_query , from + sizeof(_iexportTarget->_clientSock), querySize );

	return fromSize;	
}




bool MiyaDBMSG::validate()
{
	// 今は簡易的なチェックのみ
	// if( ntohs( _header->_messageLen ))  ;

	if( strncmp( _header->_id ,  MIYA_DB_ID , 2 ) != 0  ) return false;

	return true	;
}



MiyaDBMSG::MiyaDBMSG( unsigned int msgLen )
{
	_payload = new unsigned char[ msgLen - sizeof(_header) ];
}



MiyaDBMSG::~MiyaDBMSG( unsigned int msgLen )
{
	delete _payload;
}






bool ConnectionManager::startMonitor( int listenSock )
{
	int activeSock;
	int listenSock;
	int *connectedSock;
	std::vector<int> connectedSockList;
if( (listenSock = setupListenTCPSock()) < 0 ) return false;

	 int activeCnt;
   struct timeval tv;
   tv.tv_sec = 5;   tv.tv_usec = 0;


			for(;;)
			{
				FD_ZERO( &_fds._readFDS ); // クリア
				// accept対象のlistenSockをFDへ追加
				FD_SET( listenSock , &_fds._readFDS );
				_fds._nfds = listenSock; // スタート時はリッスンしているソケットしかないため


				if( (activeCnt = select( _fds._nfds + 1 , _fds._ndfs , nullptr , nullptr , &tv )) < -1 ){
					if( errno == EBADF ){
						// 無効になっているソケットディスクリプタを探し出す
						activeSock;
					}
				}


				for( int i=0; i < activeCnt ; i++ ){
					for( int j=0; j<10; j++){
						// 更新があったソケットディスクリプタの個数処理を回す
						if( /* ソケットディスクリプタがリッスンソケットだったら */ ){
							connectedSock = new int;
							*connectedSock = accept();
							connectedSockList.push_back( *connectedSock );
						}

						else
						{
							uint16_t msgLen;
							recvBuff = recv( activeSock , &msgLen, sizeof(msgLen), MSG_PEEK ); 

							MiyaDBMSG *msg = new MiyaDBMSG( static_cast<unsigned int>(msgLen) );
							recvBuff = recv( activeSock , msg , sizeof(msgLen), 0 ); 
							if( !(msg->validate()) ){
								delete msg;
								continue; // 破棄する
							}
						
							// バッファに追加する。あとはデータベースマネージャーがどうにかする	
							QueryPack *queryPack = new QueryPack( activeSock , msg->payload , ntohs(msg->_header->_msgLen) - sizeof(msg->_header) );
							delete msg;

							unsigned char* exportedQueryPack; unsigned int exportedQueryPackSize;
							exportedQueryPackSize = queryPack->exportRaw( &exportQueryPack );
							delete QueryPack;

							middleBuffer->push_one( exportedQueryPack , exportedQueryPackSize , false );
						}
					}
				}
	}

}








int setupListenTCPSocket( unsigned short port )
{
	int sock;
	
	if( (sock = socket( PF_INET , SOCK_STREAM , IPPROTO_TCP )) < 0 ) return -1;

	struct sockaddr_in servAddr;
	memset( &servAddr,  0x00 , sizeof(servAddr) );
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servAddr.sin_port = htons( port );


	if( bind( sock, (struct sockaddr *)&servAddr, sizeof(servAddr) ) < 0 ) return -1;

	if( listen( sock , DEFAULT_MAX_PENDING ) < 0 ) return -1;

	return sock;
}



}; // close miya_db namespace
