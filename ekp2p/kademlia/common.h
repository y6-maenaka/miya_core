#ifndef C8BB75D6_7C97_4D61_A12B_4D635ED87C64
#define C8BB75D6_7C97_4D61_A12B_4D635ED87C64


#include <bitset>
#include "./k_routing_table.h"
#include "../../shared_components/hash/sha_hash.h"



namespace ekp2p{

BitSet_160 *CalsNodeID( unsigned char* ip );



BitSet_160* CalcNodeID( struct in_addr *ip ){
		
	unsigned char* rawNodeID, *NodeID ;
	rawNodeID = new unsigned char[4];

	memcpy( rawNodeID , ip , 4 );

	delete rawNodeID;

	return CalcNodeID( ip );
}





BitSet_160* CalcNodeID( unsigned char *ip ){

	unsigned char* NodeID;

	// 内部でmallocされている
	NodeID = hash::SHAHash( ip , 4 , "sha1" );

	BitSet_160 *b_NodeID = new BitSet_160;

	// unsigned char* to BitSet
	for (int i = 0; i < 8 * NODE_ID_LENGTH ; i++) 
		b_NodeID[i] = ((NodeID[i / 8] >> (i % 8)) & 1); 																

	free( NodeID );

	return b_NodeID;			

}






} // close ekp2p namespace


#endif // C8BB75D6_7C97_4D61_A12B_4D635ED87C64

