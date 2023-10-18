#include "header.h"


namespace ekp2p{





MSGHeader::MSGHeader()
{
	memcpy( &(this->_body.StartString) , &(MSG_StartString) , sizeof(MSG_StartString) );
	memcpy( &(this->_body.EndString) , &(MSG_EndString), sizeof(MSG_EndString) );
};




MSGHeader::MSGHeader(  unsigned int payloadSize , unsigned int kTagSize , char* extra ){
	std::cout << "Message Header Generated"	 << "\n";
	memcpy( &(this->_body.StartString) , &(MSG_StartString) , sizeof(MSG_StartString) );
	memcpy( &(this->_body.EndString) , &(MSG_EndString), sizeof(MSG_EndString) );

	_body.PayloadSize = htonl( payloadSize );
	_body.KTagSize = htonl( kTagSize );
};


void MSGHeader::headerBody( struct MSGHeader::HeaderBody *headerBody )
{
	_body = *headerBody;
};


MSGHeader::HeaderBody* MSGHeader::headerBody()
{
	return &_body;
};

// 下と同じ // ※ 不要　要修正
unsigned int MSGHeader::headerBodySize()
{ 
	return ntohl(_body.PayloadSize) + ntohl(_body.KTagSize) + sizeof(struct MSGHeader::HeaderBody); // len(payload) + len(ktag) + len(header)
}


unsigned int MSGHeader::rawMSGSize()
{
	return ntohl(_body.PayloadSize) + ntohl(_body.KTagSize) + sizeof(struct MSGHeader::HeaderBody); // len(payload) + len(ktag) + len(header)
}



unsigned char* MSGHeader::free(){ // getter
	return _body.Free;
};



void MSGHeader::payloadSize( unsigned int payloadSize ){
	_body.PayloadSize =  htonl(payloadSize);
}


unsigned int MSGHeader::payloadSize(){
	return ntohl( _body.PayloadSize );
}


void MSGHeader::kTagSize( unsigned int kTagSize ){
	_body.KTagSize = htonl( kTagSize );
}

																								
unsigned int MSGHeader::kTagSize()
{
	return ntohl( _body.KTagSize );
}


char* MSGHeader::startString(){
	return _body.StartString;
};


} // close ekp2p namespace
