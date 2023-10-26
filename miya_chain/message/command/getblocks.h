#ifndef FCE6F3F9_2AE8_424F_BAB9_5327CE395140
#define FCE6F3F9_2AE8_424F_BAB9_5327CE395140


#include "./base_command.h"

#include <string.h> // for use memset() on LinuxOS

namespace miya_chain
{


/*
    指定の地点(既知)からのブロックヘッダのハッシュを提供するinvメッセージを要求するメッセージ
    ノード起動時にまだローカルノードに存在しないブロックを取得するのに使用する
*/


struct MiyaChainMSG_GETBLOCKS : public MiyaChainPayloadFunction
{

private:
	unsigned char _version[4];
	uint32_t _hashCount; // 基本的に1-200
	unsigned char _blockHeaderHash[32]; // スタートハッシュ // 初回ネットワーク参加であればジェネシスブロックハッシュとなる
	unsigned char _stopHash[32]; // ストップハッシュを0埋めすることで，後続のブロックヘッダを個数分要求する


public:
    static constexpr unsigned char command[12] = "getblocks";

		MiyaChainMSG_GETBLOCKS();

		/* Getter*/
		size_t hashCount();

		/* Setter */ 
		void hashCount( size_t hashCount );

		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );
};











}



#endif // FCE6F3F9_2AE8_424F_BAB9_5327CE395140



