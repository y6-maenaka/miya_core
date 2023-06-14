#ifndef A1D5CA78_3358_48F5_AE55_E38D603B4958
#define A1D5CA78_3358_48F5_AE55_E38D603B4958

#include <iostream>

#include "./header.h"
#include "../../kademlia/k_tag.h"



namespace ekp2p{


class EKP2PMSG{

private:
	MSGHeader *_header;

	void* _payload; // サイズは _header->payloadSize();
	KTag *_kTag; // サイズは　_header->kTagSize():


public:

	EKP2PMSG();
	~EKP2PMSG();
	
	// EKP2PMSG( unsigned char* rawMSG , unsigned int MSGSize ); // ある程度前処理がされていることが前提のコンストラクタ
	// ~EKP2PMSG();
	
	bool toStructedMSG( unsigned char* rawMSG, unsigned int MSGSize );

	/* HEADER */
	void header( MSGHeader *header ); // setter
	MSGHeader* header(); // getter

	/* PAYLOAD */
	void payload( void* payload , unsigned int payloadSize ); //setter
	void* payload(); // getter

	/* KTAG */
	void kTag( KTag* kTag ); // setter
	KTag* kTag(); // getter
	unsigned int rawKTagSize(); // getter	

	unsigned int exportRaw( unsigned char** target );
	unsigned int exportRawSize();
};

}; // close ekp2p namespace




#endif // A1D5CA78_3358_48F5_AE55_E38D603B4958

