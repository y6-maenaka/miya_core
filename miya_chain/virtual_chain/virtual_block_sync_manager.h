#ifndef BACA1037_55A2_4961_ADC2_829D9AF7A0FA
#define BACA1037_55A2_4961_ADC2_829D9AF7A0FA


#include <vector>
#include <map>
#include <memory>
#include <iostream>

#include "../message/message.h"




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
class LightUTXOSet;
class BlockLocalStrageManager;



class VirtualBlockSyncManager
{
private:
  std::map< unsigned int ,std::shared_ptr<VirtualBlock> > _chain;

  // miyaChain更新系モジュール
  std::shared_ptr< BlockLocalStrageManager > _localStrageManager;
  std::shared_ptr< LightUTXOSet > _utxoSet;

  struct Window
  {
	public:
	  unsigned short _parallelSize = 5;
	  unsigned short _windowSize = 50;

	  unsigned int _windowHeadIndex;

	  Window( unsigned int windowHeadIndex = 0 );
  } _window;

  std::shared_ptr<StreamBufferContainer> _toRequesterSBC;

protected:
  MiyaChainCommand downloadCommand();

public:
  VirtualBlockSyncManager( std::vector<std::shared_ptr<block::BlockHeader>> virtualHeaderChainVector/* これを元にvirtual_block_chainを作成する */  );

  void add( std::shared_ptr<block::Block> target ); // 外部からブロックが到着
  bool start();
};












};

#endif //
