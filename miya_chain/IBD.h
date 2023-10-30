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


constexpr unsigned int DEFAULT_BLOCK_DOWNLOAD_AGENT_COUNT = 3; // ブロックダウンロードエージェントの起動数
constexpr unsigned int DEFAULT_DOWNLOAD_BLOCK_WINDOW_SIZE = 10; // 一つのブロックダウンロードエージェントが一回で担当するブロック数


enum class IBDState : int 
{
	BlockHeaderReceived = 1, // ブロックヘッダの取得完了
	BlockHeaderValidated, // ブロックヘッダの検証完了
	BlockBodyReceived, // ブロック本体の取得完了
  BlockBodyValidated,  // ブロックの本体検証完了
  BlockStoread, // ブロック本体の取得がが完了し,ローカルに保存済み

	NowDownloading = -1, // ダウンロード中
  Error = -2, // なんらかのエラー発生
	Init  = 0
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
	
	IBDBCB();
	void header( std::shared_ptr<block::BlockHeader> target );
};







struct BlockHashAsKey // filterのunordered_mapのキーとして使う
{
	unsigned char _blockHash[32];
	struct Hash;

	BlockHashAsKey( unsigned char* target );
	BlockHashAsKey( std::shared_ptr<unsigned char> target ) : BlockHashAsKey( target.get() ){};
	bool operator==(const BlockHashAsKey& bh ) const;
  bool operator!=(const BlockHashAsKey& bh ) const;

	void printHash() const;
};

inline BlockHashAsKey::BlockHashAsKey( unsigned char* target )
{
	memcpy( _blockHash , target , sizeof(_blockHash) );
}


inline bool BlockHashAsKey::operator==(const BlockHashAsKey& key) const {
	return memcmp( _blockHash, key._blockHash , sizeof(_blockHash) );
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
	//ダブルハッシュになって非効率的だが,unsigned char[32]をsize_tに変換する術を知らない
	return std::hash<std::string>()(bytes);
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
    struct {
			std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > _um; // layer 1(全てのヘッダ)
    } _layer1;

    struct {
			std::unordered_map< BlockHashAsKey , struct IBDBCB , BlockHashAsKey::Hash > _um; // layer 2(検証が済んでvirtualChainに取り込み待ちのヘッダ)
    } _layer2;

    struct {
      std::vector< std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator > _waitQueue; // leyer1の検証待ちキュー
      std::mutex _mtx;
      std::condition_variable _cv;

	  std::unordered_map<BlockHashAsKey, struct IBDBCB, BlockHashAsKey::Hash>::iterator find( std::shared_ptr<unsigned char> targetHash );

      struct IBDBCB pop( std::shared_ptr<unsigned char> blockHash ); // callback for virtual chain(find)
    } _validation;

    // std::shared_ptr<IBDVirtualChain> _virtualChain;
	IBDVirtualChain *_virtualChain;

public:
    IBDHeaderFilter( IBDVirtualChain *virtualChain ); // ヘッダを検証するスレッドを起動する
    void add( std::shared_ptr<block::BlockHeader> header);
    void addBatch();
};




using VirtualMiyaChain = std::vector< std::pair< std::shared_ptr<unsigned char>, struct IBDBCB> >;

class IBDVirtualChain
{
private:
    // rawKey , ibdbcb
    VirtualMiyaChain _chainVector;// チェーン順に並んだIBDリスト
	VirtualMiyaChain::const_iterator _undownloadedHead; // ダウンロードはここから行う

public:
	std::shared_mutex _mtx;
	std::condition_variable_any _cv; // 多少のオーバーヘッドを伴うらしい

    void extendChain( std::function<void( std::shared_ptr<unsigned char> )> popCallback ); // 仮想チェーンの最先端ブロックハッシュを元にポップする
	const std::shared_ptr<unsigned char> chainHead();
    const VirtualMiyaChain chainVector();

	//  ダウンロードエージェント系
	VirtualMiyaChain::const_iterator popUndownloadedHead();
	std::vector< VirtualMiyaChain::const_iterator > popUndownloadedHeadBatch( size_t count );
	void downloadDone( VirtualMiyaChain::const_iterator itr );
	static void blockDownload( IBDVirtualChain *virtualChain ,  std::shared_ptr<StreamBufferContainer> toRequesterSBC );
};




};



#endif // B91C1050_1882_4050_906E_9C3D404CBF86
