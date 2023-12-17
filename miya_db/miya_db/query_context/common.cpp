#include "common.h"




namespace miya_db
{



QueryContext::QueryContext( int type , std::shared_ptr<unsigned char> data , size_t dataLength ) : _type(type)
{
	if( data != nullptr || dataLength > 0 )
	{
		_data._body = data;
		_data._bodyLength = dataLength;
	}else{
		_data._body = nullptr;
		_data._bodyLength = dataLength;
	}

	return;
}


};
