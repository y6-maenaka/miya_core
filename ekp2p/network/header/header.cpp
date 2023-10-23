#include "header.h"


#include "../../kademlia/k_node.h"



namespace ekp2p
{



EKP2PMessageHeader::EKP2PMessageHeader()
{
	memset( &(_meta), 0x00 , sizeof(_meta) );

	memcpy( _meta._token , "MIYA" , sizeof(_meta._token) );
	_meta._version = static_cast<uint8_t>(1);
	_sourceKNodeAddr = std::make_shared<KNodeAddr>();
}





/*
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
*/



size_t EKP2PMessageHeader::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	unsigned int retLength = 0 ;

	// 書き出しサイズの算出
	retLength += sizeof( _meta );
	retLength += sizeof( struct KNodeAddr ); // sourceNodeAddr
	retLength += ((_relayKNodeAddrVector.size() * sizeof( struct KNodeAddr ))); // リレーノードサイズ

	_meta._headerLength = htons(static_cast<uint16_t>(retLength)); // ヘッダサイズの確定

	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );
	memset( (*retRaw).get() , 0x00 , retLength );
	unsigned int formatOffset = 0; 


	// Meta情報の書き出し
	memcpy( (*retRaw).get() + formatOffset , &_meta , sizeof( _meta ) ); formatOffset += sizeof( _meta ); // 

	// 送信元ノードアドレスの書き出し
	if( _sourceKNodeAddr == nullptr )	{
		memset( (*retRaw).get() + formatOffset , 0x00 , sizeof( struct KNodeAddr ) ); formatOffset += sizeof( struct KNodeAddr );
	}
	else{
		memcpy( (*retRaw).get() + formatOffset , _sourceKNodeAddr.get() ,  sizeof( struct KNodeAddr) ); formatOffset += sizeof( struct KNodeAddr ); 
	}

	// 転送ノードアドレスの書き出し
	for( auto itr : _relayKNodeAddrVector ){
		memcpy( (*retRaw).get() + formatOffset , itr.get() , sizeof( struct KNodeAddr ) ); formatOffset += sizeof( struct KNodeAddr );
	}

	return retLength;

}




/*
void EKP2PMessageHeader::importRaw( void *rawHeader )
{
	memcpy( &_meta, rawHeader , sizeof(_meta) );

	unsigned int kAddrCount = (rawHeaderSize - sizeof(_meta)) / sizeof(struct KNodeAddr); 

	if( kAddrCount >= 1 )
	{
		KNodeAddr *sourceNodeAddr = new KNodeAddr;
		memcpy( sourceNodeAddr , static_cast<unsigned char*>(rawHeader) + sizeof(_meta) , sizeof(struct KNodeAddr) );
		_sourceNodeAddr = std::make_shared<KNodeAddr>( *sourceNodeAddr );
	}

	for( int i=0; i<kAddrCount-1; i++ )
	{
		KNodeAddr *relayKNodeAddr = new KNodeAddr;
		memcpy( relayKNodeAddr , static_cast<unsigned char*>(rawHeader) + sizeof(_meta) + sizeof(struct KNodeAddr) + (sizeof(struct KNodeAddr)*i) , sizeof(struct KNodeAddr) );
		_relayKNodeAddrVector.push_back( std::make_shared<KNodeAddr>(*relayKNodeAddr) );
	}
}
*/


bool EKP2PMessageHeader::importRawSequentially( std::shared_ptr<unsigned char> fromRaw )
{
	size_t currentPtr = 0;

	// _meta領域の取り込み
	memcpy( &_meta, fromRaw.get() , sizeof(_meta) ); currentPtr += sizeof(_meta);

	// sourceNodeAddrの取り込み	
	memcpy( _sourceKNodeAddr.get(), fromRaw.get() + currentPtr , sizeof( struct KNodeAddr) );

	size_t relayKNodeAddrCount = (headerLength() - sizeof(_meta) - sizeof( struct KNodeAddr)) / sizeof(struct KNodeAddr);
	for( int i=0; i<relayKNodeAddrCount; i++ )
	{
		std::shared_ptr<KNodeAddr> nodeAddr = std::make_shared<KNodeAddr>();
		memcpy( nodeAddr.get(), fromRaw.get() + currentPtr , sizeof(struct KNodeAddr)); currentPtr += sizeof(struct KNodeAddr);
		_relayKNodeAddrVector.push_back( nodeAddr );
	}

	return true;
}






void EKP2PMessageHeader::sourceKNodeAddr( std::shared_ptr<KNodeAddr> nodeAddr )
{
	if( nodeAddr == nullptr )
		_sourceKNodeAddr = nullptr;

	else 
		_sourceKNodeAddr = nodeAddr;
}


void EKP2PMessageHeader::relayKNodeAddrVector( std::vector<std::shared_ptr<KNodeAddr>> targetVector )
{
	for( auto itr : targetVector ){
		_relayKNodeAddrVector.push_back(itr);
	}
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
size_t EKP2PMessageHeader::headerLength()
{
	return static_cast<size_t>(ntohs(_meta._headerLength));
	// return static_cast<unsigned short>(_meta._headerLength);
}


unsigned short EKP2PMessageHeader::payloadLength()
{
	return static_cast<unsigned short>(ntohs(_meta._payloadLength));
}



unsigned short EKP2PMessageHeader::protocol()
{
	return static_cast<unsigned short>(ntohs(_meta._protocol));
}


unsigned short EKP2PMessageHeader::rpcType()
{
	return static_cast<unsigned short>( ntohs(_meta._rpcType) );
}




std::shared_ptr<KNodeAddr> EKP2PMessageHeader::sourceKNodeAddr()
{
	return _sourceKNodeAddr;
}





std::vector< std::shared_ptr<KNodeAddr> > EKP2PMessageHeader::relayKNodeAddrVector()
{
	return _relayKNodeAddrVector;
}




unsigned char* EKP2PMessageHeader::token()
{
	return _meta._token;
}




void EKP2PMessageHeader::printRaw()
{
	std::cout << "\x1b[31m" << "token :: ";
	for( int i=0; i<sizeof(_meta._token); i++){
		printf("%02X", _meta._token[i]);
	} std::cout << "\x1b[39m" << "\n";

	std::cout << "\x1b[32m" << "header length :: ";
	unsigned char cHeaderLength[sizeof(_meta._headerLength)]; memcpy( &cHeaderLength , &(_meta._headerLength), sizeof(_meta._headerLength) );
	for( int i=0; i<sizeof(cHeaderLength); i++ ){
		printf("%02X", cHeaderLength[i] );
	} std::cout << "\x1b[39m" << "\n";

	std::cout << "\x1b[33m" << "payload length :: ";
	unsigned char cPayloadLength[sizeof(_meta._payloadLength)]; memcpy( &cPayloadLength , &(_meta._payloadLength), sizeof(_meta._payloadLength));
	for( int i=0; i<sizeof(cPayloadLength); i++){
		printf("%02X", cPayloadLength[i]);
	} std::cout << "\x1b[39m" << "\n";

	std::cout << "rpcType :: ";
	unsigned char cRPCType[sizeof(_meta._rpcType)]; memcpy( &cRPCType, &(_meta._rpcType), sizeof(_meta._rpcType) );
	for( int i=0; i<sizeof(cRPCType); i++ ){
		printf("%02X", cRPCType[i]);
	} std::cout << "\n";


	std::cout << "\x1b[34m" << "protocol :: ";
	unsigned char cProtocol[sizeof(_meta._protocol)]; memcpy( &cProtocol, &(_meta._protocol), sizeof(_meta._protocol) );
	for( int i=0; i<sizeof(cProtocol); i++ ){
		printf("%02X", cProtocol[i]);
	} std::cout << "\x1b[39m" << "\n";


	std::cout << "\x1b[35m" << "versino :: ";
	unsigned char cVersion[sizeof(_meta._version)]; memcpy( &cVersion, &(_meta._version), sizeof(_meta._version));
	for( int i=0; i<sizeof(cVersion); i++){
		printf("%02X", cVersion[i]);
	} std::cout << "\x1b[39m" << "\n";


	std::cout << "\x1b[31m" << "relay k node addr count :: ";
	unsigned char cRelayKNodeAddrCount[sizeof(_meta._relayKNodeAddrCount)]; memcpy( &cRelayKNodeAddrCount , &(_meta._relayKNodeAddrCount), sizeof(_meta._relayKNodeAddrCount) );
	for( int i=0; i<sizeof(_meta._relayKNodeAddrCount); i++){
		printf("%02X", cRelayKNodeAddrCount[i]);
	} std::cout << "\x1b[39m" << "\n";
	




	/* 送信元KNodeAddr*/
	std::cout << "\x1b[36m" << "source k node addr :: ";
	if( _sourceKNodeAddr == nullptr ){
		std::shared_ptr<unsigned char> emptyKNodeAddr = std::shared_ptr<unsigned char>( new unsigned char[sizeof(struct KNodeAddr)] );
		memset( emptyKNodeAddr.get(), 0x00 , sizeof( struct KNodeAddr ) );
		for( int i=0; i<sizeof(struct KNodeAddr); i++){
			printf("%02X" , emptyKNodeAddr.get()[i]);
		}
	}
	else{
		std::shared_ptr<unsigned char> rawSourceKNodeAddr; size_t rawSourceKNodeAddrLength;
		rawSourceKNodeAddrLength = _sourceKNodeAddr->exportRaw( &rawSourceKNodeAddr );
		for( int i=0; i<rawSourceKNodeAddrLength; i ++ ){
			printf("%02X", rawSourceKNodeAddr.get()[i]);
		} 
	} std::cout << "\x1b[39m" << "\n";


	for( auto itr : _relayKNodeAddrVector ){
		std::cout << "relay k node addr :: ";
		std::shared_ptr<unsigned char> rawRelayKNodeAddr; size_t rawRelayKNodeAddrLength;
		rawRelayKNodeAddrLength = itr->exportRaw( &rawRelayKNodeAddr );
		for( int i=0; i<rawRelayKNodeAddrLength; i++ ){
			printf("%02X", rawRelayKNodeAddr.get()[i]);
		} std::cout << "\n";
	}


}

};
