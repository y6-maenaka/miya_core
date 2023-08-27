#include "header.h"


#include "../../kademlia/k_node.h"



namespace ekp2p
{



EKP2PMessageHeader::EKP2PMessageHeader()
{
	memcpy( _meta._token , "MIYA" , sizeof(_meta._token) );
	_meta._version = static_cast<uint8_t>(1);
}






unsigned int EKP2PMessageHeader::exportRaw( unsigned char** ret )
{
	unsigned char retLength = 0 ;

	// 書き出しサイズの算出
	if( _sourceNodeAddr != nullptr ) retLength += sizeof( struct KNodeAddr );		
	retLength += (_relayKNodeAddrVector.size() * sizeof( struct KNodeAddr ));

	retLength += sizeof( _meta ); 

	_meta._headerLength = static_cast<uint16_t>(retLength); // ヘッダサイズの確定
	
	*ret = new unsigned char[retLength]; unsigned int formatOffset = 0;
	memset( *ret , 0x00 , retLength );

	// Meta情報の書き出し
	memcpy( *ret + formatOffset , &_meta , sizeof( _meta ) ); formatOffset += sizeof( _meta );

	// 送信元ノードアドレスの書き出し
	if( _sourceNodeAddr != nullptr )	
		memcpy( *ret , _sourceNodeAddr.get() ,  sizeof( struct KNodeAddr) ); formatOffset += sizeof( struct KNodeAddr ); 

	// 転送ノードアドレスの書き出し
	for( auto itr : _relayKNodeAddrVector ){
		memcpy( *ret + formatOffset , itr.get() , sizeof( struct KNodeAddr ) ); formatOffset += sizeof( struct KNodeAddr );
	}

	return retLength;
}




void EKP2PMessageHeader::sourceNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr )
{
	if( nodeAddr == nullptr )
		_sourceNodeAddr = nullptr;

	else 
		_sourceNodeAddr = nodeAddr;
}




bool EKP2PMessageHeader::validate()
{
	std::cout << "--------------" << "\n";
	for( int i=0 ; i<4; i++)
	{
		printf("%02X", _meta._token[i]);
	} std::cout << "\n";
	std::cout << "--------------" << "\n";


	if( memcmp( _meta._token , "MIYA", 4 ) == 0 ) return true;
	
	return false;
}




/* Getter */
unsigned short EKP2PMessageHeader::headerLength()
{
	return static_cast<unsigned short>(_meta._headerLength);
}


unsigned short EKP2PMessageHeader::payloadLength()
{
	return static_cast<unsigned short>(_meta._payloadLength);
}



};
