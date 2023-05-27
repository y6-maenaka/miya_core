#include "k_tag_pack.h"


namespace ekp2p{




unsigned int KTagPack::exportRaw( unsigned char **ret )
{

}



unsigned int KTagPack::importRaw( unsigned char* from , unsigned int fromSize )
{

	unsigned int rawKTagSize = fromSize - sizeof(_iexportTarget._relatedSocketManager);

	memcpy( _iexportTarget._relatedSocketManager  , from , sizeof(_iexportTarget._relatedSocketManage) );


	_iexportTarget._rawKTag = new unsigned char[ rawKTagSize ];
	memcpy( _iexportTarget._rawKTag , from + sizeof(_iexportTarget._relatedSocketManager), rawKTagSize );

	_rawKTagSize = rawKTagSize;

	return fromSize;
}



unsigned int KTagPack::exportRaw( unsigned char **ret )
{

	unsigned int exportSize = _rawKTagSize + sizeof(_iexportTarget._relatedSocketManager);


	*ret = new unsigned char[ exportSize ];

	memcpy( *ret , _iexportTarget._relatedSocketManager , sizeof(_iexportTarget._relatedSocketManager) );
	memcpy( *ret + sizeof(_iexportTarget._relatedSocketManager) , _iexportTarget._rawKTag , _rawKTagSize );

	return exportSize;


}








}; // close ekp2p namespace
