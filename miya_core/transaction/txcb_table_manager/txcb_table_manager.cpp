#include "txcb_table_manager.h"

#include "./txcb/txcb.h"

namespace tx{


TxCBTableManager::TxCBTableManager(){
	return;
}




bool TxCBTableManager::add( P2PKH *tx )
{

	TxCB *newTxCB = new TxCB( tx );

	return _rootTable.add( newTxCB );
}



P2PKH* TxCBTableManager::find( unsigned char* txID )
{

	TxCB *hitTx = _rootTable.find( txID );

	if( hitTx == nullptr ){

		//ローカルの検索
		//
		//他のノードから取り寄せ
		
	}

	return hitTx->tx();
}


unsigned int TxCBTableManager::convertSymbol( unsigned char symbol )
{
	// ここに投入されるunsigned char symbolは0x000F以下であることが保証されている ...はず
	return static_cast<unsigned int>(symbol);
}





unsigned char TxCBTableManager::getSymbol( unsigned char *id , unsigned int layer )
{	
	unsigned char symbolByte;
	unsigned char symbol = 0x00;
	symbolByte = static_cast<unsigned char>( id[ layer / 2 ] & 0xFF );

	symbol = ( (layer % 2) == 0 ) ? ((symbolByte & 0x0F) >> 4) : (symbolByte & 0x0F);

	return symbol;
}





bool TxCBTableManager::txcmp( unsigned char* txID_1, unsigned char* txID_2 )
{

	for( int i=0 ; i < 20 ; i++ )
	{

		if( (txID_1[i] ^ txID_2[i]) == 0x0000 ) continue;

		return false;
	}

	return true;
}




}; // close tx namespace
