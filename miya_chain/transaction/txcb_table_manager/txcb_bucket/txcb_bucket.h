#ifndef E7EA68A9_965A_4A61_AB71_676770AE4BD6
#define E7EA68A9_965A_4A61_AB71_676770AE4BD6


#include <map>
#include <vector>

namespace tx{

struct TxCBBucketElem;
class TxCBTable;
class TxCB;
class P2PKH;


constexpr int DEFAULT_SCALE_SIZE  = 100;




class TxCBBucket{

private:

	unsigned int _scaleSize;

	unsigned char _symbol;
	TxCBTable *_parentTable;

	TxCB *_startCB;


public:

	TxCBBucket( unsigned char symbol , TxCBTable *parentTable );

	bool push( TxCB *txcb );
	TxCB *find( unsigned char *txID );


	void autoScale( TxCB *startCB );
};







}; // close tx namespace


#endif // E7EA68A9_965A_4A61_AB71_676770AE4BD6
