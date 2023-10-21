#include "k_tag.h"

#include "./node.h"
#include "./common.h"


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
	_KTagMeta.Protocol = htons(0);
	return;
}





KTag::KTag( void* rawKTag, unsigned int kTagSize ){
	importRaw( rawKTag , kTagSize ); // 取り込み
};



KTag::KTag( KAddr *targetKAddr )
{
	addKAddr( targetKAddr );
}



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



void KTag::importRaw( void* rawKTag , unsigned int kTagSize )
{
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
	//_KTagMeta.KAddrCnt += 1;
	_KTagMeta.KAddrCnt = _kAddrList.size();
}



void KTag::addKAddr( Node *node )
{
	addKAddr( node->kAddr() );
}



void KTag::addKAddrBatch( std::vector< Node*> *nodeVector )
{
	for( auto itr : *nodeVector )
	{
		addKAddr( itr );
	}
}




std::vector< KAddr* > *KTag::kAddrVector()
{
	return &_kAddrList;
}




















KAddr::KAddr(){

	// zero clear
	_inAddr._ipv4 = htonl(0);
	_inAddr._port = htons(0);
}



KAddr::KAddr( sockaddr_in *addr ) // import
{
	_inAddr._ipv4 = addr->sin_addr.s_addr;
	_inAddr._port = addr->sin_port;

	unsigned char *nodeID; unsigned int nodeIDSize = 0;
	nodeIDSize = calcNodeID( this , &nodeID ); // nodeIDの算出
	memcpy( _nodeID , nodeID , nodeIDSize );
	OPENSSL_free( nodeID );
}


Node* KAddr::toNode( SocketManager *socketManager )
{

	Node* node = new Node( this );

	if( socketManager != nullptr )
		node->socketManager( socketManager );

	return node;
};


unsigned char* KAddr::nodeID()
{
	return _nodeID;
}


void KAddr::nodeID( unsigned char *nodeID )
{
	memcpy( _nodeID , nodeID , sizeof(_nodeID) );
}








}; // close ekp2p namespace


