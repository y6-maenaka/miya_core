#ifndef F23E7726_96EF_4653_AA2A_BC9025A1C8A0
#define F23E7726_96EF_4653_AA2A_BC9025A1C8A0


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <cmath>


#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h> // htons用 使っていい？





namespace block
{
	struct Block;
};


namespace tx
{
	struct P2PKH;
};



struct SBSegment;
class StreamBuffer;
class StreamBufferContainer;
class MiyaDBSBClient;



namespace miya_chain
{
	


constexpr long long DEFAULT_BLK_REV_MAX_BYTES = 128 * 1024 * 1024; // std::pow(2,27); // 128MB
constexpr unsigned short BLK_REV_META_BLOCK_SIZE = 100; // メタ情報の領域(先頭から)




// access free
struct BlockContainer
{
//private:
public:

	struct Meta
	{
		unsigned char _magicBytes[4];
		uint32_t _size;
		uint32_t _txCount;
	} _meta;
	struct std::shared_ptr<block::Block> _block;


	/* 固定長で制限しているため,本家とはずらしている
		| magic bytes | size |
		| tx count           | 
		|    block header    |
		______________________
		| transactions       |
		|                    |
		|                    |
	*/

public:
	BlockContainer();
	BlockContainer( std::shared_ptr<block::Block> block );
	
	size_t size();
	size_t txCount();

	size_t exportRawFormated( std::shared_ptr<unsigned char> *retRaw );
};











//  バイナリデータ保存時にエンディアン変換は行わない
class LocalFileController
{
	// page_size をまたがるときも考慮しなけれなならない
private:

	struct Meta
	{
		uint32_t _actualDataSize;
		uint32_t _timestamp; // latest update

		void actualDataSize( size_t target );
		size_t actualDataSize();
			
	};
	struct Meta* _meta; // メタ情報はここにマッピングされる

	int _fd;
	struct stat _st;
	long long _systemPageSize;
	uint32_t _leastPageCacheOffset;

	struct 
	{
		void* _addr;
		size_t _size;
	} _mapping;

	std::string _file;

public:
	LocalFileController( std::string filePath ); // 管理するファイルパスを渡す
	std::shared_ptr<BlockContainer> read( off_t offset ); 
	long write( std::shared_ptr<BlockContainer> container ); // -1 : このファイルは満杯次のファイルパスを要求する

	std::string file();
	~LocalFileController();
};
















// アクセスフリー
struct BlockIndex
{
	// key
	unsigned char blockHash[32];


	// value of blk & rev
	uint16_t fileIndex; 

	// value of blk
	uint32_t blockOffset;
	uint32_t blockSize; // 一応

	// value of rev
	uint32_t reversalOffset;
	uint32_t reversalSize; // 一応

	uint32_t timestamp;

	unsigned char reserved[20]; // 予約領域

	/*
	(key) : blockHash
	(value) : 
					- blockFile ( blkxxxxx.dat )
					- blockOffset
					- blockSize

					- revFile ( revxxxxx.dat )
					- revOffset
					- revSize

					- timestamp
	*/

};







class BlkFileManager : public LocalFileController
{
public:
	BlkFileManager( std::string filePath ) : LocalFileController(filePath) {};
};



class RevFileManager : public LocalFileController // undo データ(ファイル)管理コンポーネント
{
public:
	RevFileManager( std::string filePath ) : LocalFileController(filePath) {};
};




class BlockLocalStrageManager
{
private:

	//std::shared_ptr<StreamBufferContainer> _toIndexDBSBC;
	std::shared_ptr<MiyaDBSBClient> _miyaDBClient;

	std::shared_ptr<BlkFileManager> _blkFileManager; // 直近でアクセスしたファイルマネージャーをキャッシュしておく
	std::shared_ptr<RevFileManager> _revFileManager; // 直近でアクセスしたファイルマネージャーをキャッシュしておく

public:
	BlockLocalStrageManager( std::shared_ptr<StreamBufferContainer> toIndexDBSBC , std::shared_ptr<StreamBufferContainer> fromIndexDBSBC );
	
	void writeBlock( std::shared_ptr<block::Block> targetBlock ); // 保存はブロック単位
	std::shared_ptr<block::Block> readBlock( std::shared_ptr<unsigned char> blockHash );

	std::shared_ptr<tx::P2PKH> readTx( std::shared_ptr<unsigned char> txHash );
};






};

#endif // F23E7726_96EF_4653_AA2A_BC9025A1C8A0


