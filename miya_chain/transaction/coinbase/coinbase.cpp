#include "coinbase.h"

#include "../tx/tx_in.h"
#include "../tx/tx_out.h"


namespace tx
{


Coinbase::Coinbase( unsigned int height , std::shared_ptr<unsigned char> text , unsigned int textLength )
{   
  _body._txIn = std::shared_ptr<TxIn>( new TxIn );
	_body._txIn->toCoinbaseInput( height , text , textLength );


}




unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	return 0;
}


}
