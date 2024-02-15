#ifndef BACA1037_55A2_4961_ADC2_829D9AF7A0FA
#define BACA1037_55A2_4961_ADC2_829D9AF7A0FA


#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <array>

#include "../message/message.h"
#include "../message/command/inv/inv.h"




class StreamBufferContainer;
struct SBSegment;


namespace block
{
  struct Block;
  struct BlockHeader;
};




namespace miya_chain
{

struct VirtualBlock;
struct UTXO;
class LightUTXOSet;
class BlockLocalStrageManager;

constexpr unsigned short BLOCK_REQUEST_TIMEOUT_SECOND = 3;
constexpr unsigned short DEFAULT_WINDOW_SIZE = 50;
constexpr unsigned short ALLOWED_RETRANSMISSION_COUNT = 4;






class VirtualBlockSyncManager
{
private:
  std::map< unsigned int ,std::shared_ptr<VirtualBlock> > _chain;
  // std::map< unsigned int , std::shared_ptr<block::BlockHeader> > _chain;

  // miyaChain更新系モジュール
  BlockLocalStrageManager *_localStrageManager;
  std::shared_ptr< LightUTXOSet > _utxoSet;

  struct UnChaindedWindow  // 親に操作される(のみ)
  {
    public:
      unsigned short _windowSize = 50; // 一回のブロックリクエスト個数
      unsigned short _parallelSize = 5; // パケットの送信先個数( 冗長さ )

	  std::vector< std::pair< std::shared_ptr<unsigned char>, std::pair< bool ,std::shared_ptr<block::Block>> > > _segmentVector;
	  //  segment : ( ブロックハッシュ, ( 本体がローカル由来(否か) , ブロック本体ポインタ ) )

      unsigned int _windowHeadIndex; // windowHeadIndex(ウィンドウ始点) - windowHeadeIndex + windowSize(ウィンドウ終点)
    
      void syncArrivedAt();

      UnChaindedWindow( unsigned short windowSize = DEFAULT_WINDOW_SIZE );

	  std::pair< bool , std::shared_ptr<block::Block> > at( unsigned int index ); 
	  void add( std::shared_ptr<block::Block> target ); // arrive
      bool isComplete();

	  unsigned short size();

	  void setup( std::vector< std::shared_ptr<VirtualBlock> > fromVBVector ); // virtual_chain_vectorからwindowを作成する
	  std::vector< std::shared_ptr<unsigned char> > unDownloadedBlockHashVector();
  
	  void __print();
  } _unChaindedWindow;

  std::shared_ptr<StreamBufferContainer> _toRequesterSBC;
  unsigned int _verifiedIndex; // 検証済みブロックのインデックス(_chainのインデックス)
 
  std::condition_variable _cv;
  std::mutex _mtx;
  
  int _status = 0;
  uint32_t _sendedAt; // 最終リクエスト送信時間
  void syncSendedAt(); // sendedAtを現在時刻で更新する
  unsigned int sendElapsedTime() const; // 最終リクエスト送信経過時間 最後にリクエストコマンドを送信してからの経過時間(s)
  
protected:
  std::pair< MiyaChainCommand, const char* > downloadCommand(); // 受信済みブロックを考慮してブロックリクエストコマンドを生成する
  void sendRequestSyncedCommand(); // windowの状態に従ってリクエストコマンドを送信する
 
  bool downloadWindow( int allowedRetransmissionCount = ALLOWED_RETRANSMISSION_COUNT ); // (segment設定済みのwindowの内セグメントのダウンロードを完了させる) ダウンロードが終了するまでブロッキングする
  void setupWindow( std::vector<std::shared_ptr<VirtualBlock>> fromVBVector );

public:
  VirtualBlockSyncManager( std::vector<std::shared_ptr<block::BlockHeader>> virtualHeaderChainVector/* これを元にvirtual_block_chainを作成する */ , BlockLocalStrageManager *localStrageManager , std::shared_ptr<StreamBufferContainer> toRequesterSBC );

  void add( std::shared_ptr<block::Block> target ); // 外部からブロックが到着
  void start();
};












};

#endif //
