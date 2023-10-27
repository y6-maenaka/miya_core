#ifndef F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55
#define F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55


#include <memory>
#include <iostream>
#include <chrono>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../miya_db/miya_db/database_manager.h"


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;






constexpr char* CHAINSTATE_FILE_PATH = "../miya_chain/miya_coin/chainstate/chainstate.st";





namespace miya_chain
{


class MiyaChainMessageBrocker;







struct MiyaChainState // 最先端のブロックハッシュなどをローカルファイルと同期しながら提供する
											// 常に読み書きを伴うのでいっそのことマッピングしてしまう
{
private:

	struct  ChainMeta
	{
		unsigned char _chainHead[32];
		uint32_t _heigth = 0;
		uint32_t _timestamp;
	} ;

	ChainMeta *_chainMeta; // ここにマッピングしたアドレスをキャストする

	long long _systemPageSize = 0;
	void* _mappedPtrHead;
	int _stateFileFD;
	struct stat _filestat;

public:
	MiyaChainState(); // 寄贈時にchain_stateファイルを読み込む
	void update( std::shared_ptr<unsigned char> blockHash , unsigned short height ); // ファイルにも書き込む

	const unsigned char* chainHead();
	unsigned int height();
	~MiyaChainState();
};







class MiyaChainManager
{

private:
	std::shared_ptr<StreamBufferContainer> _toEKP2PBrokerSBC;
	miya_db::DatabaseManager _dbManager;

	/* SBSegment ルーティング設定 */
	struct 
	{
		std::shared_ptr<StreamBufferContainer> _toBrokerSBC;
		std::shared_ptr<MiyaChainMessageBrocker> _broker;
	} _brokerDaemon;



	/* データベース関連 */
	struct 
	{
		std::shared_ptr<StreamBufferContainer> _toBlockIndexDBSBC;
		std::shared_ptr<StreamBufferContainer> _fromBlockIndexDBSBC;
	} _blockIndexDB;

	struct 
	{
		std::shared_ptr<StreamBufferContainer> _toUTXOSetDBSBC;
		std::shared_ptr<StreamBufferContainer> _fromUTXOSetDBSBC;
	} _utxoSetDB; 


public:
	int init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC );
	int start();

	std::shared_ptr<StreamBufferContainer> toBrokerSBC();
	bool IBD(); // Initial Block Download

	// first : toBlockIndexDBSBC ,  second : fromBlockIndexDBSBC
	std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer>> blockIndexDBSBCPair();
	std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer>> utxoSetDBSBCPair();
};







};




#endif // F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55


