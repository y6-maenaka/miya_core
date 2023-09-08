#ifndef A47BB5B9_83E1_4B92_B24B_150C5105164C
#define A47BB5B9_83E1_4B92_B24B_150C5105164C


#include <memory>





namespace miya_chain
{





struct BlockDataRequestMessage
{
	unsigned char _blockHash[32];
	uint16_t _txSequenceFrom; // 指定シーケンス番号以降のトランザクションを要求する

// methods
	void blockHash( std::shared_ptr<unsigned char> from );
	unsigned short exportRaw( std::shared_ptr<unsigned char> ret );
} __attribute__((packed));






struct BlockDataResponseMessage
{
	unsigned char _blockHash[32];
	uint16_t _txSequenceTo; // 指定シーケンス番号以降のトランザクションを含んだメッセージ

// methods	
	unsigned short exportRaw( std::shared_ptr<unsigned char> ret );

	// メッセージを送信する際はmiya_chainメッセージでラップする必要がある
} __attribute__((packed)); 





};



#endif
