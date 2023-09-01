#include "node.h"

#include "./k_tag.h"

#include "../network/socket_manager/socket_manager.h"
//#include "./k_routing_table.h"
#include "../network/message/message.h"
#include "./common.h"
#include "../../shared_components/hash/sha_hash.h"

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

	if( myNodeID == NULL || _kAddr->nodeID() == NULL ) return NULL;

	unsigned char* nodeXORDistance = new unsigned char[20];
	unsigned char* NodeID = _kAddr->nodeID();

	for( int i=0; i<20; i++){
		nodeXORDistance[i] = ((myNodeID[i]) ^ ( NodeID[i]));
	}

	return nodeXORDistance;
}



	
uint32_t Node::ip() // getter
{
	return _kAddr->_inAddr._ipv4;
}


void Node::ip( uint32_t ip ){ // setter from unsigned char*
	//memcpy( _kAddr->_IPv4 , ip , 4 );
	_kAddr->_inAddr._ipv4 = ip;
	//_ip = inet_addr( ip );
}



Node::Node() 
{
	_leastSocketManager = NULL;

};


/* 
Node::Node( struct in_addr *addr ) // initialize
{
	_leastSocketManager = NULL;
	_kAddr = new KAddr( addr );

};
*/




Node::Node( KAddr* kAddr, SocketManager *relatedSocketManager )
{
	if( relatedSocketManager != nullptr )
		_leastSocketManager = relatedSocketManager;
	_leastSocketManager = nullptr; // 二重で代入していることになるかな

	_kAddr = kAddr;

	// kAddrがすでに持っているはず
	// _NodeID = kAddr->IPv4;
	//unsigned char *nodeID;
	// nodeID = hash::SHAHash( kAddr->_IPv4, 4, "sha1" );
	// _kAddr->nodeID( nodeID );
	//OPENSSL_free( nodeID );
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




/*
unsigned char* Node::nodeID()
{
	return _NodeID;
}
*/



bool Node::SendPING()
{
	
	/* generate message */
	
	// _leaseSocketManager->send( send ) ;
	
	KTag *kTag = new KTag;

	kTag->addKAddr( _kAddr ); // 自身のkAddrを追加
	kTag->protocol( 2 );

	// PINGMSGのみだからpayloadはなし
	send( nullptr , 0 , kTag );
	

	delete kTag;
	return true;
}



bool Node::SendPONG()
{
	KTag *kTag = new KTag;
	kTag->protocol( 3 );

	send( NULL , 0 , kTag );

	delete kTag;
	return true;
}

bool Node::SendFIND_NODE()
{
	KTag *kTag = new KTag;

	kTag->addKAddr( _kAddr ); // 自身のkAddrを追加
	kTag->protocol( 4 );

	send( NULL , 0 , kTag );

	delete kTag;
	return true;
}



KAddr* Node::kAddr() // getter
{
	return _kAddr;
}





int Node::send( EKP2PMSG *msg )
{
	unsigned char* rawMSG; unsigned int rawMSGSize;
	rawMSGSize = msg->exportRaw( &rawMSG );

	unsigned int sendSize;
	sendSize = _leastSocketManager->send( rawMSG, rawMSGSize );

	delete rawMSG;
	return sendSize;
}



int Node::send( unsigned char *payload , unsigned int payloadSize , KTag *kTag )
{
	SocketManager *socketManager = _leastSocketManager;

	if( socketManager == NULL ){
		std::cout << " socket Manager not seted" << "\n";
		return -1;
	}

	int sendSize =  -1;	

	EKP2PMSG msg;
	msg.payload( payload , payloadSize ); // payloadがnullptrでもうまくやってくれる
	msg.kTag( kTag ); // kTagがnullptrでもうまくやってくれる

	return send( &msg );

	/*	
	unsigned char *rawMSG; unsigned int rawMSGSize;
	rawMSGSize = msg.exportRaw( &rawMSG );

	sendSize = socketManager->send( rawMSG , rawMSGSize );

	return sendSize;
	*/
}



int Node::send( unsigned char *msg , unsigned int msgSize )
{
	SocketManager *socketManager = _leastSocketManager;

	if( socketManager == nullptr ){
		std::cout << " socket Manager not seted" << "\n";
		return -1;
	}

	
	int sendSize = -1;
	sendSize = socketManager->send( msg , msgSize );

	return sendSize;

}









}; // close ekp2p namespace
