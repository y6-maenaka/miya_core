#include "node.h"

#include "./k_tag.h"

#include "../network/socket_manager/socket_manager.h"

//#include "../hash/sha_hash.h"


namespace ekp2p{




short int Node::calcBranch( unsigned char* myNodeID ){

	// 無駄処理
	unsigned char *nodeXORDistance = calcBranchRaw( myNodeID );

	if( nodeXORDistance == NULL ) return -1;
	
	int cnt = 0;
	for( int i=0; i<160; i++ )
		cnt += ((nodeXORDistance[i / 8] >> (i % 8)) & 1);


	// b_NODEXORDistance.to_ulong();

	return static_cast<short int>( cnt );	
};



unsigned char* Node::calcBranchRaw( unsigned char* myNodeID )
{

	if( myNodeID == NULL || _NodeID == NULL ) return NULL;

	unsigned char* nodeXORDistance = new unsigned char[20];

	for( int i=0; i<20; i++){
		nodeXORDistance[i] = ((myNodeID[i]) ^ (_NodeID[i]));
	}

	return nodeXORDistance;
}



	
unsigned char* Node::ip() // getter
{
	return _kAddr->IPv4;
}


void Node::ip( unsigned char* ip ){ // setter from unsigned char*
	memcpy( _kAddr->IPv4 , ip , 4 );
	//_ip = inet_addr( ip );
}



Node::Node() 
{
	_leastSocketManager = NULL;

};



Node::Node( struct in_addr *ip )
{
	_leastSocketManager = NULL;

	memcpy( _kAddr->IPv4, ip, 4 ); // set _ip

	_NodeID = hash::SHAHash( _kAddr->IPv4 , 4 , "sha1" );// set _NodeID

	
};




Node::Node( KAddr* kAddr )
{
	_leastSocketManager = NULL;

	_kAddr = kAddr;

	// _NodeID = kAddr->IPv4;
	_NodeID = hash::SHAHash( kAddr->IPv4, 4, "sha1" );

};



Node::~Node(){
	//delete _b_NodeID;
}




void Node::socketManager( SocketManager *socketManager ) // setter
{
	_leastSocketManager = socketManager;
}




SocketManager* Node::socketManager() // getter
{ 
	return _leastSocketManager;
}





unsigned char* Node::nodeID()
{
	return _NodeID;
}



bool Node::SendPING()
{
	
	/* generate message */
	
	// _leaseSocketManager->send( send ) ;
	
	KTag *kTag = new KTag;

	kTag->addKAddr( _kAddr ); // 自身のkAddrを追加
	kTag->protocol( 2 );

	// PINGMSGのみだからpayloadはなし
	send( NULL , 0 , kTag , this->_leastSocketManager );
	

	delete kTag;
	return true;
}



bool Node::SendPONG()
{
	KTag *kTag = new KTag;
	kTag->protocol( 3 );

	send( NULL , 0 , kTag, this->_leastSocketManager );

	delete kTag;
	return true;
}

bool Node::SendFIND_NODE()
{
	KTag *kTag = new KTag;

	kTag->addKAddr( _kAddr ); // 自身のkAddrを追加
	kTag->protocol( 4 );

	send( NULL , 0 , kTag , this->_leastSocketManager );

	delete kTag;
	return true;
}



KAddr* Node::kAddr() // getter
{
	return _kAddr;
}





}; // close ekp2p namespace
