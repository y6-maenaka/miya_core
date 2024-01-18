#ifndef FCE6F3F9_2AE8_424F_BAB9_5327CE395140
#define FCE6F3F9_2AE8_424F_BAB9_5327CE395140


#include "./base_command.h"

#include <iostream>
#include <string.h> // for use memset() on LinuxOS


namespace block
{
	struct BlockHeader;
}

namespace miya_chain
{


/*
    指定の地点(既知)からのブロックヘッダのハッシュを提供するinvメッセージを要求するメッセージ
    ノード起動時にまだローカルノードに存在しないブロックを取得するのに使用する
*/


struct MiyaChainMSG_GETBLOCKS : public MiyaChainPayloadFunction
{

private:
	struct __attribute__((packed))
	{
		unsigned char _version[4];
		uint32_t _hashCount; // 基本的に1-200
		unsigned char _blockHeaderHash[32]; // スタートハッシュ // 初回ネットワーク参加であればジェネシスブロックハッシュとなる
		unsigned char _stopHash[32]; // ストップハッシュを0埋めすることで，後続のブロックヘッダを個数分要求する
	} _body ;

public:
    static constexpr char command[12] = "getblocks";

		MiyaChainMSG_GETBLOCKS( size_t hashCount = 200 );
		MiyaChainMSG_GETBLOCKS( std::shared_ptr<block::BlockHeader> startHashHeader /* このヘッダーをprevHashにとるblock(blockHash)をリクエストする*/ ,size_t hashCount = 200 ); //　コマンド作成時は基本的にこれを使う

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

		void print();
};











}



#endif // FCE6F3F9_2AE8_424F_BAB9_5327CE395140
