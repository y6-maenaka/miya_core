#ifndef D9C3DA28_8439_4D06_96A0_3D2D2B5EF526
#define D9C3DA28_8439_4D06_96A0_3D2D2B5EF526


#include <memory>



namespace miya_chain
{
	class BlockLocalStrageManager;
};







namespace block
{

struct Block;








class BlockCIterator // const iterator 
{
private:
	std::shared_ptr<Block> _body = nullptr;
	std::shared_ptr<unsigned char> _blockHashCache;
	long long _heigthCache;

	std::shared_ptr<miya_chain::BlockLocalStrageManager> _localStrageManager = nullptr;
	
	void setupFromBlockHash( std::shared_ptr<unsigned char> from );

public:
	BlockCIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<unsigned char> bodyBlockHash );
	BlockCIterator( std::shared_ptr<miya_chain::BlockLocalStrageManager> localStrageManager , std::shared_ptr<Block> body ); 
	std::shared_ptr<Block> operator*(); // デリファレンス
	BlockCIterator &operator--();

	std::shared_ptr<unsigned char> blockHash() const;
	long long heigth() const;

	bool operator==( const BlockCIterator &si ) const;
	bool operator!=( const BlockCIterator &si ) const;

};











};




#endif // D9C3DA28_8439_4D06_96A0_3D2D2B5EF526


