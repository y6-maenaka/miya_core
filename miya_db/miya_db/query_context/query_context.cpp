#include "query_context.h"

namespace miya_db
{



void QueryContext::key( std::shared_ptr<unsigned char> key , size_t keyLength )
{
	_data._key = key;
	_data._keyLength = keyLength;
}



void QueryContext::value( std::shared_ptr<unsigned char> data , size_t dataLength )
{
	_data._value = data;
	_data._valueLength = dataLength;

	return;
}


};
