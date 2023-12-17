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

int QueryContext::type()
{
	return _type;
}

void QueryContext::type( int target )
{
	_type = target;
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

uint32_t QueryContext::id()
{
	return _id;
}

void QueryContext::id( uint32_t target )
{
	_id = target;
}

void QueryContext::registryIndex( short target )
{
	_safe._index = target;
}

short QueryContext::registryIndex()
{
	return _safe._index;
}

uint64_t QueryContext::timestamp()
{
	return _timestamp;
}

void QueryContext::timestamp( uint64_t target )
{
	_timestamp = target;
}




};
