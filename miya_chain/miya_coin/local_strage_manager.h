#ifndef F23E7726_96EF_4653_AA2A_BC9025A1C8A0
#define F23E7726_96EF_4653_AA2A_BC9025A1C8A0


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <cmath>
#include <regex>
#include <algorithm>
#include <set>
#include <cassert>


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

struct UTXO;



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
	std::shared_ptr<block::Block> _block;


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

	std::shared_ptr<block::Block> block();

	void importMetaSequentially( std::shared_ptr<unsigned char> fromRaw ); // 一応取り込みメソッドも用意しておく
	void importBlockSequentially( std::shared_ptr<unsigned char> fromRaw );

	// ローカルファイル格納形式に書き出す
	size_t exportRawFormated( std::shared_ptr<unsigned char> *retRaw );
};




struct UndoContainer // undo:巻き戻しデータ
{
//private:
public:
	struct Meta
	{
		unsigned char _magicBytes[4]; // 今の所機能させていない
		uint32_t _size;
		uint32_t _count;
	} _meta;

	std::vector< std::shared_ptr<miya_chain::UTXO> > _utxoVector;

public:
	UndoContainer();
	UndoContainer( std::shared_ptr<block::Block> blockFrom ); // このブロック内のトランザクションからundoを作成する

	size_t size();
	size_t count();

	std::vector< std::shared_ptr<miya_chain::UTXO> > utxoVector();

	void importMetaSequentially( std::shared_ptr<unsigned char> fromRaw );
	void importUndoFromRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ); // metaを正常に格納している必要がある

	// ローカルファイル格納形式に書き出す
	size_t exportRawFormated( std::shared_ptr<unsigned char> *retRaw ); // 各UTXOはbsonエンコード形式 全体もjson配列形式で書き出す
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

	} __attribute__((packed));

	Meta* _meta; // メタ情報はここにマッピングされる

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
	~LocalFileController();

	// std::shared_ptr<BlockContainer> read( off_t offset );
	long write( std::shared_ptr<unsigned char> data , size_t dataLength );
	//(return):実際に読み込みたデータ数   (data):読み込みデータ , (size):読み込みデータ長, (offset):開始地点
	size_t read( std::shared_ptr<unsigned char> *data, size_t size ,off_t offset );

	// std::shared_ptr<BlockContainer> read( off_t offset );


	std::string file();
};
















// アクセスフリー
struct BlockIndex // ブロック関連ファイル格納フォーマット(ヘッダー)  データ本体の前に付与される
{
	uint16_t fileIndex;  // value of blk & rev
	uint32_t blockOffset; // value of blk
	uint32_t reversalOffset; // value of rev

	uint32_t timestamp;

	unsigned char reserved[20]; // 予約領域

	/*
	(value) :
					- blockFile ( blkxxxxx.dat )
					- blockOffset

					- revFile ( revxxxxx.dat )
					- revOffset

					- timestamp
	*/

	BlockIndex();
	size_t blkOffset();
	size_t revOffset();
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	void importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength ); // 一応メソッドとして実装しておく
};







class BlkFileManager : public LocalFileController
{
public:
	BlkFileManager( std::string filePath ) : LocalFileController(filePath) {};

	long write( std::shared_ptr<BlockContainer> container );
	std::shared_ptr<BlockContainer> read( off_t offset );
};



class RevFileManager : public LocalFileController // undo データ(ファイル)管理コンポーネント
{
public:
	RevFileManager( std::string filePath ) : LocalFileController(filePath) {};

	long write( std::shared_ptr<block::Block> targetBlock );
	std::shared_ptr<UndoContainer> read( off_t offset );
};




class BlockLocalStrageManager
{
private:
	//std::shared_ptr<StreamBufferContainer> _toIndexDBSBC;
	std::shared_ptr<MiyaDBSBClient> _miyaDBClient;

	std::shared_ptr<BlkFileManager> _blkFileManager; // 直近でアクセスしたファイルマネージャーをキャッシュしておく
	std::shared_ptr<RevFileManager> _revFileManager; // 直近でアクセスしたファイルマネージャーをキャッシュしておく

	unsigned int _lastIndex = 0;
public:
	BlockLocalStrageManager( std::shared_ptr<StreamBufferContainer> toIndexDBSBC , std::shared_ptr<StreamBufferContainer> fromIndexDBSBC );

	// ブロック操作
	void writeBlock( std::shared_ptr<block::Block> targetBlock ); // 保存はブロック単位  Revファイルも同時に作成される
	std::shared_ptr<block::Block> readBlock( std::shared_ptr<unsigned char> blockHash );
	// トランザクション操作
	std::shared_ptr<tx::P2PKH> readTx( std::shared_ptr<unsigned char> txHash );
	
	// Rev操作
	std::vector< std::shared_ptr<UTXO> > readUndo( std::shared_ptr<unsigned char> blockHash );


	std::vector< std::shared_ptr<UTXO> > releaseBlock( std::shared_ptr<unsigned char> blockHash );  // ブロック・Revが共に管理外となる
																																																	// undoはリターンする
};

/*
 ブロックの保存と同時にrev(UNDO)ファイルを作成して保存しておく
 ブロックリリース時にはrevファイルからUTXOリストをリターンする
*/




};

#endif // F23E7726_96EF_4653_AA2A_BC9025A1C8A0
