#ifndef AAE74D51_1FC2_4B79_9A8E_33318A1294BE
#define AAE74D51_1FC2_4B79_9A8E_33318A1294BE


#include <iostream>
#include <unistd.h>



namespace ekp2p{


class SocketManager;



struct KTagPack
{

	struct IExportTarget
	{
		SocketManager* _relatedSocketManager;
		unsigned char* _rawKTag;

	} _iexportTarget;
	
	unsigned int _rawKTagSize;


	KTagPack(){};

	KTagPack( SocketManager *relatedSocketManager , unsigned char *rawKTag  ,unsigned int rawKTagSize ) // set
	{
		_iexportTarget._relatedSocketManager = relatedSocketManager;

		_iexportTarget._rawKTag = new unsigned char[ rawKTagSize ];
		memcpy( _iexportTarget._rawKTag , rawKTag , rawKTagSize );

		_rawKTagSize = rawKTagSize;
	};

	~KTagPack(){
		delete _iexportTarget._rawKTag;
	};


	unsigned int exportRaw( unsigned char **ret ); // これで連続した領域に配置し直す
	unsigned int importRaw( unsigned char *from , unsigned int fromSize );


	unsigned char* rawKTag(){
		return _iexportTarget._rawKTag;
	};
	unsigned int rawKTagSize(){
		return _rawKTagSize;
	};

	SocketManager* relatedSocketManager(){
		return _iexportTarget. _relatedSocketManager;
	};

};




}; // close ekp2p namespace


#endif // AAE74D51_1FC2_4B79_9A8E_33318A1294BE

