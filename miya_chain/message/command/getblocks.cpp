#include "getblocks.h"



namespace miya_chain
{




/* Getter */
size_t MiyaChainMSG_GETBLOCKS::hashCount()
{
	return static_cast<size_t>( ntohs(_hashCount) );
}




/* Setter */
void MiyaChainMSG_GETBLOCKS::hashCount( size_t hashCount )
{
	_hashCount = htons( hashCount );
}






MiyaChainMSG_GETBLOCKS::MiyaChainMSG_GETBLOCKS()
{
	memset( this , 0x00 , sizeof(struct MiyaChainMSG_GETBLOCKS) );
}



size_t MiyaChainMSG_GETBLOCKS::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct MiyaChainMSG_GETBLOCKS)] );
	memcpy( (*retRaw).get() , this,  sizeof(struct MiyaChainMSG_GETBLOCKS) );

	return sizeof(struct MiyaChainMSG_GETBLOCKS);
}




bool MiyaChainMSG_GETBLOCKS::importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	return false;
}







};

