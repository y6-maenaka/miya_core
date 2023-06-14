#include "message.h"

#include "./header.h"


namespace ekp2p{


EKP2PMSG::EKP2PMSG(){ 
	_header = new MSGHeader( 0 , 0 , nullptr ); // ( payloadSize , kTagSize , 予備 )
	_payload = nullptr;
	_kTag = nullptr;

}



EKP2PMSG::~EKP2PMSG()
{
	if( _payload != nullptr ) delete _payload;
	if( _kTag != nullptr ) delete _kTag;
}


/*
// raw to structure
EKP2PMSG::EKP2PMSG( unsigned char* rawMSG ,unsigned int MSGSize ){

	int cpyPtr = 0;

	_header = new MSGHeader;
	memcpy( _header->headerBody() , rawMSG, sizeof( struct MSGHeader::HeaderBody ) ); cpyPtr += sizeof( struct MSGHeader::HeaderBody );

	_payload = (void *)malloc( _header->payloadSize() );

	memset( _payload, 0x0, _header->payloadSize() ); // 念の為
	memcpy( _payload , rawMSG + cpyPtr, _header->payloadSize() ); cpyPtr += _header->payloadSize();
	
	if( _header->kTagSize() > 0 ){
		_kTag = new KTag( rawMSG + cpyPtr , _header->kTagSize() ); 
		// memcpy( _kTag, rawMSG + cpyPtr , _header->kTagSize()); cpyPtr += _header->kTagSize();
	}
};
*/



bool EKP2PMSG::toStructedMSG( unsigned char* rawMSG , unsigned int MSGSize ){

	int cpyPtr = 0;

	if( MSGSize <= 0 || rawMSG == nullptr )
	{
		// _header = nullptr; // payload=0, ktag=0 のheaderを保持させたまま
		return false;
	}

	//_header = new MSGHeader;
	memcpy( _header->headerBody() , rawMSG, sizeof( struct MSGHeader::HeaderBody ) ); cpyPtr += sizeof( struct MSGHeader::HeaderBody );

	// _payload = (void *)malloc( _header->payloadSize() );
	_payload = new unsigned char[ _header->payloadSize() ];
	memset( _payload, 0x0, _header->payloadSize() ); // 念の為
																									 
	memcpy( _payload , rawMSG + cpyPtr, _header->payloadSize() ); cpyPtr += _header->payloadSize();
	
	if( _header->kTagSize() > 0 )
	{
		_kTag = new KTag( rawMSG + cpyPtr , _header->kTagSize() ); 
		// memcpy( _kTag, rawMSG + cpyPtr , _header->kTagSize()); cpyPtr += _header->kTagSize();
	}

	return true;
}




/* HEADER */
void EKP2PMSG::header( MSGHeader *header ) // setter
{
	_header = header;
}



MSGHeader* EKP2PMSG::header() // getter
{
	return _header;
}






/* PAYLOAD */
void EKP2PMSG::payload( void* payload, unsigned int payloadSize )
{
	if( _header == nullptr ){ // もしくはエラー返却にするか？
		_header = new MSGHeader;		
	}

	if( payloadSize <= 0 || payload == nullptr ){
		_header->payloadSize(0);
		return;
	}

	_payload = payload; 
	_header->payloadSize( payloadSize );

}




void* EKP2PMSG::payload()
{
	return _payload; 
}





/* KTAG */
void EKP2PMSG::kTag( KTag* kTag  ) // setter
{
	if( _header == nullptr ){ // もしくはエラー返却にするか？
		_header = new MSGHeader;
	}

	if( kTag == nullptr ) return;

	_kTag = kTag;

	return;
}



KTag* EKP2PMSG::kTag()
{
	return _kTag;
}



unsigned int EKP2PMSG::exportRaw( unsigned char **target )
{

	/* header の各要素をセットを確認 */
	if( _kTag != NULL )   _header->kTagSize( _kTag->exportRawSize() );
	// if( _payload != NULL )  これはセットされる時に必ず入力されている


	*target = (unsigned char *)malloc( _header->rawMSGSize() );

	unsigned int cpyPtr = 0; 
	memcpy( *target , _header->headerBody() , sizeof( MSGHeader::HeaderBody ));  cpyPtr += sizeof( MSGHeader::HeaderBody );

	if( _header->payloadSize() > 0 )
		memcpy( *target  + cpyPtr , _payload , _header->payloadSize() ); cpyPtr += _header->payloadSize();

	/*
	// 単にこれではダメ ※要改良
	if( _header->kTagSize() > 0 )
		memcpy( *target  + cpyPtr , _kTag , _header->kTagSize() );  cpyPtr += _header->kTagSize();
	*/

  // 多分ここで深刻なエラーは出ないはず	
	if( _kTag != NULL )
	{
		unsigned char *exportBuff = NULL;
		unsigned int exportBuffSize = 0;
		exportBuffSize = _kTag->exportRaw( &exportBuff );

		memcpy( *target + cpyPtr , exportBuff, exportBuffSize   );
		delete exportBuff;
	}

	return cpyPtr;
}




unsigned int EKP2PMSG::exportRawSize(){
	return _header->headerBodySize();
}


}; // close ekp2p namespace
