#include "miya_core_manager.h"


#include "../shared_components/stream_buffer/stream_buffer.h"
#include "../shared_components/stream_buffer/stream_buffer_container.h"

#include "./daemon/broker/broker.h"

namespace miya_chain
{






MiyaChainState::MiyaChainState()
{
	_systemPageSize = sysconf (_SC_PAGESIZE); // 本アーキテクチャのページサイズを取得

	_stateFileFD = open( CHAINSTATE_FILE_PATH , O_RDWR | O_CREAT, (mode_t)(0600) );
	if( stat( CHAINSTATE_FILE_PATH , &_filestat  )  != 0 ) return;

	std::cout << "Open ChainStateFile with FD :: " << _stateFileFD << "\n";
	std::cout << "[ Attention ] system page size :: " << _systemPageSize << "\n";


	if( _stateFileFD < 0 ) return;
	off_t currentFileSize = _filestat.st_size;
	if( currentFileSize < sizeof(struct MiyaChainState::ChainMeta) ) ftruncate( _stateFileFD , sizeof(struct MiyaChainState::ChainMeta) ); // 領域が不足していたら確保する

	_mappedPtrHead = mmap( NULL , sizeof(_chainMeta), PROT_READ | PROT_WRITE ,MAP_SHARED, _stateFileFD , 0 ); // offセットは0
	_chainMeta = (struct MiyaChainState::ChainMeta *)( _mappedPtrHead );
}


void MiyaChainState::update( std::shared_ptr<unsigned char> blockHash , unsigned short height )
{
	memcpy( &(_chainMeta->_chainHead)  , blockHash.get() , 32 );
	_chainMeta->_heigth = htons(height); // エンディアン変換しなくても別にいいかな
	_chainMeta->_timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	msync( _mappedPtrHead , sizeof(struct MiyaChainState::ChainMeta) , MS_SYNC ); // 同期を忘れない
}

MiyaChainState::~MiyaChainState()
{
	munmap( _mappedPtrHead , sizeof(struct MiyaChainState::ChainMeta) );
	close(_stateFileFD);
}












int MiyaChainManager::init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC )
{


	/*
	

			一旦すべてのデータベースを起動する

	*/





	_toEKP2PBrokerSBC = toEKP2PBrokerSBC;

	// ブローカーの起動
	_brokerDaemon._toBrokerSBC = std::make_shared<StreamBufferContainer>();

  
	_brokerDaemon._broker = std::make_shared<MiyaChainMessageBrocker>( _brokerDaemon._toBrokerSBC , _toEKP2PBrokerSBC );

	std::cout << "MiyaChainManager initialize successfully done" << "\n";
}



int MiyaChainManager::start()
{
	_brokerDaemon._broker->start();
}



std::shared_ptr<StreamBufferContainer> MiyaChainManager::toBrokerSBC()
{
	return _brokerDaemon._toBrokerSBC;
}





};
