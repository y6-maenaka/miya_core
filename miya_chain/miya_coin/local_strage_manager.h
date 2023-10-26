#ifndef F23E7726_96EF_4653_AA2A_BC9025A1C8A0
#define F23E7726_96EF_4653_AA2A_BC9025A1C8A0


#include <iostream>
#include <memory>




namespace block
{
	struct Block;
};


namespace tx
{
	struct P2PKH;
};





namespace miya_chain
{
	


struct BlockContainer
{
private:
	unsigned char _magicBytes[4];
	uint32_t _size;
	struct std::shared_ptr<block::Block> _block;

public:
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
};






struct BlockIndexFormat
{
	/*
	(key) : blockHash
	(value) : 
					- blockFile ( blkxxxxx.dat )
					- blockOffset

					- revFile ( revxxxxx.dat )
					- revOffset

					- timestamp
	*/
};






class LocalStrageManager
{
private:

public:
	std::shared_ptr<block::Block> getBlock( std::shared_ptr<unsigned char> blockHash );
	std::shared_ptr<tx::P2PKH> getTx( std::shared_ptr<unsigned char> txHash );
};






};

#endif // F23E7726_96EF_4653_AA2A_BC9025A1C8A0



