#include "headers.h"


namespace chain
{



void MiyaChainMSG_HEADERS::count( size_t target )
{
	_body._count = static_cast<uint32_t>( target );
}



std::vector< std::shared_ptr<block::BlockHeader> > MiyaChainMSG_HEADERS::headersVector()
{
	return _body._headers;
}




size_t MiyaChainMSG_HEADERS::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	size_t rawTxsLength = 0;
	std::vector< std::pair<std::shared_ptr<unsigned char> , size_t> > rawTxVector;
	for( auto itr : _body._headers )
	{
		std::shared_ptr<unsigned char> rawTx; size_t rawTxLength = 0;
		rawTxLength = itr->exportRaw( &rawTx );
		rawTxVector.push_back( std::make_pair(rawTx , rawTxLength) );
		rawTxsLength += rawTxLength;
	}

	size_t retLength = sizeof(_body._count) + rawTxsLength;
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
	
	// 書き出し
	size_t formatPtr = 0;
	memcpy( (*retRaw).get() + formatPtr , &(_body._count) , sizeof(_body._count) );
	for( auto itr : rawTxVector ){
		memcpy( (*retRaw).get() + formatPtr , (itr.first).get() , itr.second );
		formatPtr += itr.second;
	}

	return formatPtr;

}


};
