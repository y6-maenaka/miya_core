#include "miya_chain_manager.h"
#include "./miya_chain_state.h"

#include "./block/block.h"
#include "./block/block_header.h"
#include "./miya_coin/local_strage_manager.h"



namespace miya_chain
{





std::shared_ptr<unsigned char> MiyaChainState::ChainMeta::blockHash()
{
  std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_chainHead)] );
  memcpy( ret.get(), _chainHead , sizeof(_chainHead) );
  return ret;
}


MiyaChainState::MiyaChainState( std::shared_ptr<BlockLocalStrageManager> localStrageManager )
{
	std::cout << "This is MiyaChainState コンストラクタ" << "\n";

	assert(localStrageManager != nullptr );
	_localStrageManager = localStrageManager;


	// MiyaChainState( std::shared_ptr<BlockLocalStrageManager> localStrageManager ); // 寄贈時にchain_stateファイルを読み込む


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



	std::cout << "\n --- [ Chain State ] ---------------------------------- " << "\n";
	printf( "| ChainState File Mapped with :: %p\n", _chainMeta );
	std::cout << "| Chain Heade Block :: ";
	/*
	for( int i=0; i<32; i++ ){
		printf("%02X", chainHead().get()[i] );
	} std::cout << "\n";

	std::cout << "| Heigth :: " << height() << "\n";
	*/ // イレテータから取得する
	std::cout << "---------------------------------------------------------------------" << "\n";


}


void MiyaChainState::update( std::shared_ptr<unsigned char> blockHash , unsigned short height )
{
	memcpy( &(_chainMeta->_chainHead)  , blockHash.get() , 32 );
	_chainMeta->_heigth = static_cast<uint32_t>(height); // エンディアン変換しなくても別にいいかな
	_chainMeta->_timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	memset( &(_chainMeta->_version) , 0x00 , sizeof(_chainMeta->_version) );
	msync( _mappedPtrHead , sizeof(struct MiyaChainState::ChainMeta) , MS_SYNC ); // 同期を忘れない

	std::cout << "chain state updated" << "\n";
}


std::shared_ptr<BlockChainIterator> MiyaChainState::latestBlockIterator()
{
  std::shared_ptr<BlockChainIterator> ret = std::shared_ptr<BlockChainIterator>( new BlockChainIterator( _localStrageManager , _chainMeta->blockHash() ) );
  
  return ret;
}


MiyaChainState::~MiyaChainState()
{
	std::cout << "~MiyaChainState() called" << "\n";
	munmap( _mappedPtrHead , sizeof(struct MiyaChainState::ChainMeta) );
	close(_stateFileFD);
}

/*
std::shared_ptr<unsigned char> MiyaChainState::chainHead()
{
	std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>(new unsigned char[sizeof(_chainMeta->_chainHead)] ); // コピー領域
	memcpy( ret.get() , _chainMeta->_chainHead , sizeof(_chainMeta->_chainHead) );
	return ret;
}
*/

/*
unsigned int MiyaChainState::height()
{
	return static_cast<unsigned int>(_chainMeta->_heigth);
}
*/














};
