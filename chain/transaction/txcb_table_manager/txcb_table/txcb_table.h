#ifndef E88C085E_5C54_4AEC_A1DC_41834FCFF6D0
#define E88C085E_5C54_4AEC_A1DC_41834FCFF6D0



#include <vector>
#include <map>




namespace tx{

class TxCBBucket;
class TxCB;
struct P2PKH;




enum class GENERIC_LIST_ELEM_TYPE : int {
	BUCKET,
	TABLE,
};





class TxCBTable{

private:
	unsigned int _layer;

	// 範囲外を作りたくないため、vectorではなく配列を採用
	std::pair< int , void* > _genericList[16]; // あとでprivateに移動


public:
	void swapGenericListElem( std::pair<int,void*> elem, int index );
	
	unsigned int layer();

	TxCBTable( unsigned int layer = 0 );

	/* control */
	bool add( TxCB *txcb );
	TxCB *find( unsigned char *txID );

};






}; // close tx namespace



#endif // E88C085E_5C54_4AEC_A1DC_41834FCFF6D0
