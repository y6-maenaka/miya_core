#ifndef FCE6F3F9_2AE8_424F_BAB9_5327CE395140
#define FCE6F3F9_2AE8_424F_BAB9_5327CE395140


#include <iostream>
#include <string.h> // for use memset() on LinuxOS
#include <chain/block/block.params.hpp>


namespace chain
{

/*
    指定の地点(既知)からのブロックヘッダのハッシュを提供するinvメッセージを要求するメッセージ
    ノード起動時にまだローカルノードに存在しないブロックを取得するのに使用する
*/

struct MiyaCoreMSG_GETBLOCKS 
{

private:
  struct __attribute__((packed))
  {
	  unsigned char _version[4];
	  uint32_t _hash_count; // 基本的に1-200
	  //unsigned char _startHash[32]; // スタートハッシュ // 初回ネットワーク参加であればジェネシスブロックハッシュとなる
	  block_id _start_hash;
	  // unsigned char _stopHash[32]; // ストップハッシュを0埋めすることで，後続のブロックヘッダを個数分要求する
	  block_id _end_hash;
  } _body ;

public:
  static constexpr char command[12] = "getblocks";

  MiyaCoreMSG_GETBLOCKS( size_t hashCount = 200 );
  MiyaCoreMSG_GETBLOCKS( block_id start_hash, block_id end_hash = block_id::invalid(), unsigned short hash_count = 200 );
  MiyaCoreMSG_GETBLOCKS( std::shared_ptr<struct BlockHeader> startHashHeader /* このヘッダーをprevHashにとるblock(blockHash)をリクエストする*/ ,size_t hashCount = 200 ); //　コマンド作成時は基本的にこれを使う

  /* Getter*/
  size_t hashCount();

  /* Setter */
  void hashCount( size_t hashCount );
  void startHash( std::shared_ptr<unsigned char> blockHash ); // blocHeaderHash
  void startHash( const void* blockHash );

  void endHash( std::shared_ptr<unsigned char> blocHash );
  void endHash( const void* blockHash );

  size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
  bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  void __print();
  
  std::vector<std::uint8_t> export_to_binary() const;
};


MiyaCoreMSG_GETBLOCKS::MiyaCoreMSG_GETBLOCKS( size_t hashCount )
{
	memset( this , 0x00 , sizeof(struct MiyaCoreMSG_GETBLOCKS) );
	this->hashCount( hashCount );
}


MiyaCoreMSG_GETBLOCKS::MiyaCoreMSG_GETBLOCKS( block_id start_hash, block_id end_hash, unsigned short hash_count )
{
  _body._start_hash = start_hash;
  _body._end_hash = end_hash;
  _body._hash_count = hash_count;
}



/*

// Getter
size_t MiyaCoreMSG_GETBLOCKS::hashCount()
{
	return static_cast<size_t>( _body._hashCount );
}

// Setter
void MiyaCoreMSG_GETBLOCKS::hashCount( size_t hashCount )
{
	_body._hashCount =  static_cast<uint32_t>(hashCount);
}


void MiyaCoreMSG_GETBLOCKS::startHash( const void* blockHash )
{
	memcpy( _body._startHash , blockHash , sizeof(_body._startHash) );
}

void MiyaCoreMSG_GETBLOCKS::startHash( std::shared_ptr<unsigned char> blockHash )
{
	this->startHash( blockHash.get() );
}

void MiyaCoreMSG_GETBLOCKS::endHash( const void* blockHash )
{
	if( blockHash == nullptr ){
		memset( _body._stopHash , 0x00 , sizeof(_body._stopHash) );
		return;
	}

	memcpy( _body._stopHash , blockHash , sizeof(_body._stopHash) );
}

void MiyaCoreMSG_GETBLOCKS::endHash( std::shared_ptr<unsigned char> blocHash )
{
	this->endHash( blocHash.get() );
}

void MiyaCoreMSG_GETBLOCKS::__print()
{
	std::cout << "version :: ";
	for(int i=0; i<sizeof(_body._version); i++)
		printf("%02X", _body._version[i] );
	std::cout << "\n";

	std::cout << "hashCount :: ";
	unsigned char hashCountBuff[sizeof(_body._hashCount)];
	memcpy( hashCountBuff, &(_body._hashCount) , sizeof(_body._hashCount) );
	for( int i=0; i<sizeof(hashCountBuff); i++ ){
		printf("%02X", hashCountBuff[i] );
	} std::cout << "\n";

	std::cout << "startHash :: ";
	for( int i=0; i<sizeof(_body._startHash); i++ ){
		printf("%02X", _body._startHash[i]);
	} std::cout << "\n";

	std::cout << "endHash :: ";
	for( int i=0; i<sizeof(_body._stopHash); i++ ){
		printf("%02X", _body._stopHash[i]);
	} std::cout << "\n";
}

*/


};


#endif // FCE6F3F9_2AE8_424F_BAB9_5327CE395140
