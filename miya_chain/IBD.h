#ifndef B91C1050_1882_4050_906E_9C3D404CBF86
#define B91C1050_1882_4050_906E_9C3D404CBF86

#include "./miya_core_manager.h"
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <cassert>
#include <chrono>


namespace block
{
struct Block;
struct BlockHeader;
};





namespace miya_chain
{


class IBDVirtualChain;
class LightUTXOSet;
class BlockLocalStrageManager;



constexpr unsigned int DEFAULT_BLOCK_DOWNLOAD_AGENT_COUNT = 3; // ブロックダウンロードエージェントの起動数
constexpr unsigned int DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE = 10; // 一つのブロックダウンロードエージェントが一回で担当するブロック数


enum class IBDState : int 
{
	BlockHeaderReceived = 1, // ブロックヘッダの取得完了
	BlockHeaderValidated, // ブロックヘッダの検証完了
	BlockHeaderNotfound,
	BlockBodyReceived, // ブロック本体の取得完了
	BlockNotfound,
  	BlockBodyValidated,  // ブロックの本体検証完了
  	BlockStoread, // ブロック本体の取得がが完了し,ローカルに保存済み

	NowDownloading = -1, // ダウンロード中
  	Error = -2, // なんらかのエラー発生
	Empty = 0
};





struct IBDBCB // (Initial Block Donwload Block Control Block)
{
	std::shared_ptr<block::Block> block;
	uint32_t height; // いらないかも
	int status; 
	// 1 : blockHashまで受信完了
	// 2 : ヘッダーまで受信完了
	// 3 : トランザクション収集中
	// 4 : 保存済み
	// -1 : エラー発生
	// -2 : 空
	
	IBDBCB();
	void header( std::shared_ptr<block::BlockHeader> target );
	void setErrorFlag();

	std::shared_ptr<unsigned char> blockHash();
	std::shared_ptr<unsigned char> prevBlock();

	void print();
};







struct BlockHashAsKey // filterのunordered_mapのキーとして使う
{
	unsigned char _blockHash[32];
	struct Hash;

	BlockHashAsKey(){};
	BlockHashAsKey( unsigned char* target );
	BlockHashAsKey( std::shared_ptr<unsigned char> target ) : BlockHashAsKey( target.get() ){};
	bool operator==(const BlockHashAsKey& bh ) const;
  bool operator!=(const BlockHashAsKey& bh ) const;

	void printHash() const;
};

inline BlockHashAsKey::BlockHashAsKey( unsigned char* target )
{
	if( target == nullptr ) return;
	memcpy( _blockHash , target , sizeof(_blockHash) );
}


inline bool BlockHashAsKey::operator==(const BlockHashAsKey& key) const {
	return (memcmp( _blockHash, key._blockHash , sizeof(_blockHash) ) == 0 );
}

inline bool BlockHashAsKey::operator!=(const BlockHashAsKey& key) const {
    return !(this->operator==(key));
}

struct BlockHashAsKey::Hash {
    std::size_t operator()(const BlockHashAsKey& key) const;
};

inline std::size_t BlockHashAsKey::Hash::operator()(const BlockHashAsKey& key) const 
{
	std::string bytes(reinterpret_cast<const char*>(key._blockHash), sizeof(key._blockHash)); 
	return std::hash<std::string>()(bytes);
	//std::cout << "KeyHash :: " << ret << "\n";
	//ダブルハッシュになって非効率的だが,unsigned char[32]をsize_tに変換する術を知らない
}

inline void BlockHashAsKey::printHash() const
{
	std::cout << "Key(BlockHash) :: ";
	for( int i=0; i<sizeof(_blockHash); i++ ){
		printf("%02X", _blockHash[i] );
	} std::cout << "\n";
}







class IBDHeaderFilter
{
private:
    struct Layer1
	{
		std::mutex _mtx;
		std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > _um; // layer 1(全てのヘッダ)
    } _layer1; // ダウンロードしなければならない全てのヘッダ情報

    struct Layer2
	{
		std::mutex _mtx;
		std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > _um; // layer 2(検証が済んでvirtualChainに取り込み待ちのヘッダをもつIBDCB)

		// 内部ibcbcのprevOutを比較対象としてpopする
		struct IBDBCB findPop( std::shared_ptr<unsigned char> blockHash ); // callback for virtual chain(find)
		void push( BlockHashAsKey blockKey , struct IBDBCB cb );
    } _layer2;

    struct Validation{
      std::vector< std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator > _waitQueue; // leyer1の検証待ちキュー
      std::mutex _mtx;
      std::condition_variable _cv;

	  std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator find( std::shared_ptr<unsigned char> targetHash );
	  void push( std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator target );
    } _validation;

    // std::shared_ptr<IBDVirtualChain> _virtualChain;
	IBDVirtualChain *_virtualChain;

public:
    IBDHeaderFilter( IBDVirtualChain *virtualChain ); // ヘッダを検証するスレッドを起動する
    void add( std::shared_ptr<block::BlockHeader> header);
    void addBatch();


	size_t sizeLayer1(); // IBDブロックの個数(layer1)
	size_t sizeLayer2();
};




using VirtualMiyaChain = std::vector< std::pair< std::shared_ptr<unsigned char>, struct IBDBCB> >;

class IBDVirtualChain
{
private:
    // rawKey , ibdbcb
    VirtualMiyaChain _chainVector;// チェーン順に並んだIBDリスト
	VirtualMiyaChain::iterator _undownloadedHead; // ダウンロードはここから行う

public:
	// 必ずローカルチェーンヘッドで初期化(をセット)する, _undownloadedHeadが正常に初期化されない
	IBDVirtualChain( std::shared_ptr<unsigned char> localChainHead , struct IBDBCB initialCB );
	std::shared_mutex _mtx;
	std::condition_variable_any _cv; // 多少のオーバーヘッドを伴うらしい

    void requestedExtendChain( std::function<struct IBDBCB( std::shared_ptr<unsigned char> )> popCallback ); // 仮想チェーンの最先端ブロックハッシュを元にポップする
	std::shared_ptr<unsigned char> chainHead();
    VirtualMiyaChain chainVector();

		// 基本的にfilterから追加される際に使われる, すでに,blockHashを持ったIBDBがキャッシュされていることが条件
	void add( std::shared_ptr<block::Block> target );  // 強制的に追加する, chainHeadを追加すること以外には基本的に使わない
	void addForce( std::shared_ptr<unsigned char> blockHash ,struct IBDBCB cb );
	void addForce( std::shared_ptr<block::Block> target );

	size_t size();

	//  ダウンロードエージェント系
	VirtualMiyaChain::iterator popUndownloadedHead();
	std::vector< VirtualMiyaChain::iterator > popUndownloadedHeadBatch( size_t count );
	void downloadDone( VirtualMiyaChain::iterator itr );
	static void blockDownload( IBDVirtualChain *virtualChain ,  std::shared_ptr<StreamBufferContainer> toRequesterSBC , std::shared_ptr<LightUTXOSet> utxoSet , std::shared_ptr<BlockLocalStrageManager> localStrageManager );
};




};



#endif // B91C1050_1882_4050_906E_9C3D404CBF86
