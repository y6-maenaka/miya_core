#ifndef AC9E3B49_B0C8_4A18_949F_A50D12B0AD92
#define AC9E3B49_B0C8_4A18_949F_A50D12B0AD92



#include <vector>
#include <array>
#include <memory>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "../../../share/stream_buffer/stream_buffer.h"

#include <node_gateway/message/message.hpp>


struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;



namespace ekp2p
{
	struct KNodeAddr;
};



namespace block
{
	struct Block;
	struct BlockHeader;
};



namespace chain
{




constexpr unsigned int BLOCK_HASH_LENGTH = (256/8);
constexpr unsigned int MAX_PENDING_QUEUE_SIZE = 5;
constexpr unsigned int MAX_PENDING_TIME = (60 * 3); // 3分間待機する



class BlockHeaderMessage;
struct BlockDataResponseMessage;
class BlockValidator;




struct BlockValidationControlBlock
{
	std::shared_ptr<unsigned char> _blockHash;

	struct 
	{
		std::shared_ptr<unsigned char> _header;

		unsigned int _dataLength;
		unsigned int _txCount;
		std::shared_ptr<unsigned char> _data;
	} _rawBlock;

	struct
	{
		std::shared_ptr<block::Block> _block;
		unsigned int _txCount = 0;
		bool _updateFlag = false;
	} _structedBlock;

	struct
	{
		std::shared_ptr<ekp2p::KNodeAddr> _sourceKNodeAddr;
		std::vector< std::shared_ptr<ekp2p::KNodeAddr> > _relayKNodeAddrVector;
		std::shared_ptr<StreamBufferContainer> _toSenderSBC;
	} _networks;

	time_t _startTime;
	BlockValidator *_validator;
	std::mutex _mtx;
	std::condition_variable _cv;

public:
	BlockValidationControlBlock( std::shared_ptr<StreamBufferContainer> senderSBC , BlockValidator *validator );

	void blockHash( unsigned char* target );
	void networks( std::shared_ptr<ekp2p::KNodeAddr> sourceKNodeAddr , std::vector< std::shared_ptr<ekp2p::KNodeAddr> > relayNodeAddr );

	void importStrucretHeader( std::shared_ptr<BlockHeaderMessage> target );
	void importRawHeader( unsigned char* rawHeader , unsigned int rawHeaderLength );
	void importRawBlock( unsigned char* rawBlock , unsigned int rawBlockLength );

	void autoStart(); 
	void txArrive( std::shared_ptr<BlockDataResponseMessage> msg ); // 検証用スレッドを起こす
	/* 1. ブロックデータの取得 // ブロックデータの検証 // ブロックデータの伝搬 */
};










struct PendingResponseQueue
{
 // 破棄時間はどうする？

private:
	std::vector< std::shared_ptr<BlockValidationControlBlock> > _bvcbQueue; // block validation control block queue
	//unsigned short _bvcbCount;

protected:
	void timeOutRefresh();

public:
	std::shared_ptr<BlockValidationControlBlock> find( unsigned char* targetHash );
	void enqueue( std::shared_ptr<BlockValidationControlBlock> target );
	std::shared_ptr<BlockValidationControlBlock> dequeue( std::shared_ptr<BlockValidationControlBlock> target );
};











class BlockValidator
{

private:
	std::shared_ptr<StreamBufferContainer> _sourceSBC;
	std::shared_ptr<StreamBufferContainer> _destinationSBC; 

	PendingResponseQueue _pendingQueue;


protected:
	
public:
	void start();

	void vlidationDone( std::shared_ptr<BlockValidationControlBlock> target );
};




/*
 // 他ノードからブロックが伝搬してきた際のチェックプロセス
  1. ブロックヘッダの受信
	2. PoWの確認
	3. 確認が取れたらブロック本体の検証のためデータを集める
	4. ブロック内のトランザクションを検証する
	5. 検証が正常終了したら他ノードへ転送する
*/


/*
 BlockMSGとGetBlockMSGは異なる
 */



};




#endif // AC9E3B49_B0C8_4A18_949F_A50D12B0AD92



