#ifndef F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55
#define F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55


#include <memory>
#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <filesystem>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <node_gateway/message/message.h>
#include <node_gateway/message/command/command_set.h>
#include <chain/chain.hpp>

#include <miya_db/miya_db/database_manager.h>
#include <chain/block_chain_iterator/block_chain_iterator.h>
#include "./IBD.h"

#include <stream_buffer/stream_buffer.h>
#include <stream_buffer/stream_buffer_container.h>

#include <chain/daemon/broker/broker.h>
#include <chain/daemon/requester/requester.h>

#include <chain/block/block.h>
#include <chain/block/block_header.h>

#include <chain/utxo_set/utxo_set.h>
#include <chain/chain_state.hpp>


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;


namespace chain
{

constexpr char* CHAINSTATE_FILE_PATH = "../chain/miya_coin/chainstate/chainstate.st";
constexpr unsigned short DEFAULT_DAEMON_FORWARDING_SBC_ID_REQUESTER = 0;
constexpr unsigned short DEFAULT_DAEMON_FORWARDING_SBC_ID_RESPONDER = 1;


class MiyaChainBrocker;
class MiyaChainRequester;
class BlockLocalStrageManager;
class LightUTXOSet;
class BlockChainIterator;


struct MiyaChainState // 最先端のブロックハッシュなどをローカルファイルと同期しながら提供する
											// 常に読み書きを伴うのでいっそのことマッピングしてしまう
{
private:
	struct ChainMeta
	{
		unsigned char _chainHead[32];
		uint32_t _heigth = 0;
		uint32_t _timestamp;
		unsigned char _version[4];  // ここではないかも

		std::shared_ptr<unsigned char> blockHash();
	} __attribute__((packed));

	ChainMeta *_chainMeta; // ここにマッピングしたアドレスをキャストする

	long long _systemPageSize = 0;
	void* _mappedPtrHead;
	int _stateFileFD;
	struct stat _filestat;

	std::shared_ptr<BlockLocalStrageManager> _localStrageManager; // block_c_iterator生成よ用

public:
	MiyaChainState( std::shared_ptr<BlockLocalStrageManager> localStrageManager ); // 寄贈時にchain_stateファイルを読み込む
	~MiyaChainState();

	void update( std::shared_ptr<unsigned char> blockHash , unsigned short height ); // ファイルにも書き込む
	std::shared_ptr<BlockChainIterator> latestBlockIterator();
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
		std::shared_ptr<MiyaChainBrocker> _broker;
	} _brokerDaemon;

	struct
	{
		std::shared_ptr<StreamBufferContainer> _toRequesterSBC;
		std::shared_ptr<MiyaChainRequester> _requester;
	} _requesterDaemon;


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

	struct  // ローカルストレージマネージャーへの呼び出しは現在の実装では同期的に行われる
	{
		std::shared_ptr<BlockLocalStrageManager> _strageManager;
	} _localStrageManager;


	std::shared_ptr<MiyaChainState> _chainState;
	std::shared_ptr<LightUTXOSet> _utxoSet;

public:
	int init( std::shared_ptr<StreamBufferContainer> toEKP2PBrokerSBC );
	int start();

	std::shared_ptr<StreamBufferContainer> toBrokerSBC();
	bool startIBD(); // Initial Block Download

	/* Getter */
	// first : toBlockIndexDBSBC ,  second : fromBlockIndexDBSBC
	std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer>> blockIndexDBSBCPair();
	// first : toUTxOSetDBSBC , second : fromUTxOSetDBSBC ※ UTxOデータベースとのやり取りは基本的にUTxOSet(クライアント)を使う事
	std::pair<std::shared_ptr<StreamBufferContainer>, std::shared_ptr<StreamBufferContainer>> utxoSetDBSBCPair();
	std::shared_ptr<MiyaChainState> chainState();
	std::shared_ptr<LightUTXOSet> utxoSet();

	std::shared_ptr<BlockLocalStrageManager> localStrageManager();
	void __unitTest( std::vector<std::shared_ptr<Block>> blocks );
};


class chain_manager 
{
public:
  void income_getdata();
  chain_manager( class BlockLocalStrageManager &block_strage_manager, std::string path_to_chainstate_sr );

private:
  class BlockLocalStrageManager &_block_strage_manager;
  class local_chain _local_chain;
  class local_chain& get_local_chain();
};


};


#endif // F9D3E3DD_AAEF_4D7F_9063_874CB7AF3E55
