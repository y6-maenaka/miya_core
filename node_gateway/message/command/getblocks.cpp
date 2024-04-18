#include "getblocks.h"



namespace chain
{



MiyaChainMSG_GETBLOCKS::MiyaChainMSG_GETBLOCKS( size_t hashCount )
{
	memset( this , 0x00 , sizeof(struct MiyaChainMSG_GETBLOCKS) );
	this->hashCount( hashCount );
}





/* Getter */
size_t MiyaChainMSG_GETBLOCKS::hashCount()
{
	return static_cast<size_t>( _body._hashCount );
}




/* Setter */
void MiyaChainMSG_GETBLOCKS::hashCount( size_t hashCount )
{
	_body._hashCount =  static_cast<uint32_t>(hashCount);
}



void MiyaChainMSG_GETBLOCKS::startHash( const void* blockHash )
{
	memcpy( _body._blockHeaderHash , blockHash , sizeof(_body._blockHeaderHash) );
}

void MiyaChainMSG_GETBLOCKS::startHash( std::shared_ptr<unsigned char> blockHash )
{
	this->startHash( blockHash.get() );
}





void MiyaChainMSG_GETBLOCKS::endHash( const void* blockHash )
{
	if( blockHash == nullptr ){
		memset( _body._stopHash , 0x00 , sizeof(_body._stopHash) );
		return;
	}

	memcpy( _body._stopHash , blockHash , sizeof(_body._stopHash) );
}


void MiyaChainMSG_GETBLOCKS::endHash( std::shared_ptr<unsigned char> blocHash )
{
	this->endHash( blocHash.get() );
}


size_t MiyaChainMSG_GETBLOCKS::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[sizeof(_body)] );
	memcpy( (*retRaw).get() , &_body,  sizeof(_body) );

	return sizeof(_body);
}




bool MiyaChainMSG_GETBLOCKS::importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	return false;
}


void MiyaChainMSG_GETBLOCKS::__print()
{
	std::cout << "version :: ";
	for(int i=0; i<sizeof(_body._version); i++)
		printf("%02X", _body._version[i] );
	std::cout << "\n";

	std::cout << "hashCount :: ";
	unsigned char hashCountBuff[sizeof(_body._hashCount)];
	memcpy( hashCountBuff, &(_body._hashCount) , sizeof(_body._hashCount) );
	for( int i=0; i<sizeof(hashCountBuff); i++ ){
		printf("%02X", hashCountBuff[i] );
	} std::cout << "\n";

	std::cout << "startHash :: ";
	for( int i=0; i<sizeof(_body._blockHeaderHash); i++ ){
		printf("%02X", _body._blockHeaderHash[i]);
	} std::cout << "\n";

	std::cout << "endHash :: ";
	for( int i=0; i<sizeof(_body._stopHash); i++ ){
		printf("%02X", _body._stopHash[i]);
	} std::cout << "\n";

}




};
