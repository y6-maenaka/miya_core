#include "inband_manager.h"

#include "../message/message.h"

#include "../../../shared_components/middle_buffer/middle_buffer.h"



namespace ekp2p{





struct KTagPack
{

	struct IExportTarget
	{
		SocketManager* _relatedSocketManager;
		unsigned char* _rawKTag;

	} _iexportTarget;
	
	unsigned int _rawKTagSize;

	KTagPack( SocketManager *relatedSocketManager , unsigned char *rawKTag  ,unsigned int rawKTagSize ) // set
	{
		_relatedSocketManager = relatedSocketManager;

		_rawKTag = new unsigned char[kTagSize];
		memcpy( _rawKTag,  rawKTag , kTagSize );
		//_rawKTag = rawKTag;

		_rawKTagSize = rawKTagSize;
	};

	~KTagPack(){
		delete rawKTag;
	};


	unsigned int exportRaw( unsigned char **ret ); // これで連続した領域に配置し直す
	unsigned int inportRaw( unsigned char *rawKTagPack );

	unsigned char* rawKTag(){
		return _iexportTarget->_rawKTag;
	};
	unsigned int rawkTagSize(){
		return _rawKTag;
	};

	SocketManager* relatedSocketManager(){
		return _relatedSocketManager;
	};

};




unsigned int KTagPack::exportRaw( unsigned char **ret )
{
	int exportSize =  sizeof(_iexportTarget->_relatedSocketManager) + _rawKTagSize ;
	*ret = unsigned char[iexportSize];
	memcpy( *ret , _iexportTarget->_relatedSocketManager , sizeof(_iexportTarget->_relatedSocketManager) );
	memcpy( *ret + sizeof(_iexportTarget->_relatedSocketManager), _rawKTag , _rawKTagSize );

	return exportSize;
}


unsigned int KTagPack::impoertRaw( unsigned char *from , unsigned int fromSize )
{
	_rawKTagSize = fromSize - sizeof(_iexportTarget->_relatedSocketManager );

	memcpy( _iexportTarget->_relatedSocketManager , from , sizeof(_iexportTarget->_relatedSocketManager) );
	memcpy( _iexportTarget->_rawKTag , from + sizeof(_iexportTarget->_relatedSocketManager), _rawKTagSize );

	return fromSize;
}




void DefaultMessageHandler( void* arg , EKP2PMSG *msg ){


	InbandManager *inbandManager = (InbandManager *)arg;
	MiddleBuffer *middleBuffer = (MiddleBuffer *)(inbandManager->_handlerArg); // handlerArgにデータが格納されている
	/* このMiddleBufferはtable_wrapper(kademli)用 */

	/*
	unsigned int rawKTagSize = msg->kTag()->exportRawSize();
	unsigned char *rawKTag = new unsigned char[rawKTagSize];
	msg->kTag()->exportRaw( &rawKTag );
	*/


	unsigned char* rawKTag; unsigned int rawKTagSize;
	rawKTagSize = msg->kTag()->exportRaw( *rawKTag );

	KTagPack *kTagPack = new KTagPack( inbandManager->_socketManager , rawKTag , rawKTagSize );
	delete rawKTag; // 中で移し替えられている
	delete msg; // KTagPackに移し替え終わり 

	unsigned char* exportedKTagPack; unsigned int exportedKTagPackSize;
	exportedKTagPackSize = ktagPack->exportRaw( &exportedKTagPack );

	if( middleBuffer != nullptr  ){ // kTagPackは単にキャストすると連続した領域に配置されないから,回収できないかも
		middleBuffer->pushOne( (void*)exportedRawKTag , exportedRawKTagSize , false );	// update k_routing_table with k_tag 
		delete exportedKTagPack; // pushしたら内部領域にコピーされる
	}

	return;
}


};
