#include "k_tag.h"

#include "./node.h"


namespace ekp2p{




bool KTag::KTagMeta::validate(){

	// 曖昧
	if( ntohs(Protocol) <= 0 || ntohs(Protocol) >= 10 ) return false;

	if( ntohs(KAddrCnt) < 0 )	 return false;

	return true;
}


unsigned short KTag::KTagMeta::kAddrCnt(){
	return ntohs( KAddrCnt );
};



KTag::KTagMeta::KTagMeta() : KAddrCnt(0)
{ 
	memset( Free , 0x0 , 12  );
};




KTag::KTag(){
	return;
}





KTag::KTag( void* rawKTag, unsigned int kTagSize ){

	memcpy( &_KTagMeta, rawKTag, sizeof( struct KTag::KTagMeta ));

	if( !(this->_KTagMeta.validate() ))
	{
		return;
	}
	
	for( int i=0; i<_KTagMeta.kAddrCnt(); i++ ){

		KAddr *targetKAddr = new KAddr;
		memcpy( targetKAddr, (unsigned char *)rawKTag + sizeof( struct KTag::KTagMeta) + ( i * sizeof( struct KAddr )), sizeof( struct KAddr ) );
			
		_kAddrList.push_back( targetKAddr );
	}

};




unsigned int KTag::exportRaw( unsigned char** ret )
{
	
	*ret  = new unsigned char[ exportRawSize() ];
	memset( *ret , 0x0 , exportRawSize() );

	int cpyPtr = 0;
	memcpy( *ret  , &_KTagMeta , sizeof( _KTagMeta ));     cpyPtr += sizeof( _KTagMeta );


	for( auto itr = _kAddrList.cbegin() ; itr != _kAddrList.cend() ; itr++  ){
		memcpy( *ret + cpyPtr , *itr , sizeof( struct KAddr ));  cpyPtr += sizeof( struct KAddr );
	}

	

	return exportRawSize();
}


unsigned int KTag::exportRawSize()
{
	// size( meta ) + (size( kaddr ) * count )
	return sizeof( struct KTag::KTagMeta ) + (sizeof( struct KAddr ) * _kAddrList.size()) ;
}


void KTag::protocol( unsigned short protocol ){
	_KTagMeta.Protocol = htons( protocol );
}


unsigned short KTag::protocol(){	
	return ntohs(_KTagMeta.Protocol);
}





void KTag::addKAddr( KAddr *kAddr )
{
	_kAddrList.push_back( kAddr );
	_KTagMeta.KAddrCnt += 1;
}



void KTag::addKAddr( Node *node ){

	_kAddrList.push_back( node->kAddr() ); 
	_KTagMeta.KAddrCnt += 1;
}





KAddr::KAddr(){
	memset( IPv4, 0x0 , sizeof( IPv4)  );
	UDPPort = htons( 0 );
	TCPPort = htons( 0 );
}



Node* KAddr::toNode( SocketManager *socketManager ){

	Node* node = new Node( this );

	if( socketManager != NULL )
		node->socketManager( socketManager );

	return node;
};




}; // close ekp2p namespace


