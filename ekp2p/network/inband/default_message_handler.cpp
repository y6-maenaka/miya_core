#include "inband_manager.h"

#include "../message/message.h"

#include "../../../shared_components/middle_buffer/middle_buffer.h"
#include "./k_tag_pack.h"
#include "./inband_manager.h"



namespace ekp2p{






int DefaultMessageHandler( void* arg , EKP2PMSG *msg ){


	BaseInbandManager *inbandManager = (BaseInbandManager *)arg;
	MiddleBuffer *middleBuffer = (MiddleBuffer *)( inbandManager->handlerArg() ); // handlerArgにデータが格納されている
	/* このMiddleBufferはtable_wrapper(kademli)用 */

	/*
	unsigned int rawKTagSize = msg->kTag()->exportRawSize();
	unsigned char *rawKTag = new unsigned char[rawKTagSize];
	msg->kTag()->exportRaw( &rawKTag );
	*/


	unsigned char* rawKTag; unsigned int rawKTagSize;
	rawKTagSize = msg->kTag()->exportRaw( &rawKTag );

	KTagPack *kTagPack = new KTagPack( inbandManager->socketManager() , rawKTag , rawKTagSize );
	delete rawKTag; // 中で移し替えられている
	delete msg; // KTagPackに移し替え終わり 

	unsigned char* exportedRawKTagPack; unsigned int exportedRawKTagPackSize;
	exportedRawKTagPackSize = kTagPack->exportRaw( &exportedRawKTagPack );

	if( middleBuffer != nullptr  ){ // kTagPackは単にキャストすると連続した領域に配置されないから,回収できないかも
		middleBuffer->pushOne( exportedRawKTagPack , exportedRawKTagPackSize , false );	// update k_routing_table with k_tag 
		delete exportedRawKTagPack; // pushしたら内部領域にコピーされる
	}

	return 0;
}


};
