#ifndef BD11FF79_FAD2_42D9_93DA_AEE328D19915
#define BD11FF79_FAD2_42D9_93DA_AEE328D19915



#include <iostream>
#include <bitset>
#include <arpa/inet.h>
#include <mutex>
#include <condition_variable>
#include <vector>
// AwaikeThread

namespace ekp2p{


class Node;
struct NodeListElem;




struct ReplacementThreadArgs
{
	NodeListElem *targetElem;
	Node *candidateNode;
	unsigned short waitTime;

	ReplacementThreadArgs( NodeListElem* _targetElem , Node* _candidateNode , unsigned short _waitTime ){
		this->targetElem = _targetElem;
		this->candidateNode = _candidateNode;
		this->waitTime = _waitTime;
	}
};






struct NodeListElem {

	struct Body
	{
		Node* _node;
		bool _isReserved;
	} _body;

		// Body():_isReserved(NULL){}; } _body;
	void swapNode( Node* targetNode );
	NodeListElem::Body* body(); // getter


	/* VARIABLE */
	NodeListElem *_nextElemPtr;
	NodeListElem *_prevElemPtr;	


	/* METHOD */

	/* ==== GETTER | SETTER ==== */
	Node* node(); // getter
	void node( Node* node ); // setter
													
	NodeListElem *next(); // getter
	void next( NodeListElem* nextElemPtr ); // setter
																				
	NodeListElem *prev(); // getter
	void prev( NodeListElem* prevElemPtr ); // setter

	void isReserved( bool isReserved ); // setter
	bool isReserved(); // getter
	
	NodeListElem( Node *targetNode );

};








class NodeList{

private:
	unsigned short _nodeCnt;
	unsigned short _listSize;

public:
	NodeListElem *_front; // あとでprivateにする

	unsigned short size();
	unsigned short count();

	NodeList( int maxNodeCnt );

	/* CONTROL */

	// [ exist : return -1 ] , [ not exist : return idx ]
	//NodeListElem *findNode( Node *targetNode );
	
	NodeListElem *front(); // getter

	/* CONTROL */
	bool push_back( Node* targetNode );           // 
	bool push_back( NodeListElem *targetElem );   //  prive に移動してupdateからの呼び出し可能にする
	void push_front( Node* targetNode );          // 

	void toRear( NodeListElem* targetElem );
	
	//std::vector<Node*> *exportNodeList(); // getter
	std::vector< Node* > *exportNodeVector(); 

	void show_Test();
};









class KBucket{

private:	
	unsigned short _branch; // ブランチ(0-160)
	std::mutex _mtx;
	std::condition_variable _cv;

public:
	NodeList *_nodeList; // move to private

	NodeList* nodeList(); // getter
	
	unsigned short branch(); // getter

	KBucket( unsigned short branch );
	// Node* nextNodePointer(); // getter

	NodeListElem* update( Node *targetNode );
	NodeListElem* find( Node *targetNode );

	// Warning
	// void add( Node *targetNode );

	Node* operator[]( int index );
	Node* speNode( int index ); // operatorと同じ

	void StartReplacementThread( NodeListElem *targetElem ,  Node *candidateNode , unsigned int waitTime = 10 );
	friend void* ReplacementThread( void* args );

	std::vector< Node* >  *exportNodeVector();
};









}; // close ekp2p namespace


#endif // BD11FF79_FAD2_42D9_93DA_AEE328D19915

