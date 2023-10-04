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




std::shared_ptr<unsigned char> QueryContext::key()
{
	return _data._key;
}


size_t QueryContext::keyLength()
{
	return _data._keyLength;
}



std::shared_ptr<unsigned char> QueryContext::value()
{
	return _data._value;
}



size_t QueryContext::valueLength()
{
	return _data._valueLength;
}










};
