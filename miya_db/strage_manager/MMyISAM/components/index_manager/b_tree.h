#ifndef B1FC59BD_B0A1_4868_807D_4E4014B7B8D2
#define B1FC59BD_B0A1_4868_807D_4E4014B7B8D2



#include <iostream>
#include <map>
#include <vector>
#include <algorithm>



namespace miya_db{

template<typename> // pre 
class BTreeNode;


constexpr int DEFAULT_NODE_THRESHOLD = 3;

template<typename T>
using ChildPtrSet = std::pair< BTreeNode<T>* , unsigned char* >;





template <typename T>
struct NodeItemSet{

	struct ItemSet{
		T **_keys;
		std::pair< BTreeNode<T>*, unsigned char*> **_child;
		unsigned char **_dataPtr;
	} _itemSet ; 
	


	NodeItemSet( int threshold ){

		_itemSet._keys = new T*[threshold]; // 分割の際に勝手がいいように保有できるキー数 + 1														
		_itemSet._child = new std::pair< BTreeNode<T>*, unsigned char *>*[threshold + 1];
		for( int i=0; i<threshold + 1 ; i++) _itemSet._child[i] = new std::pair< BTreeNode<T>* , unsigned char*>(nullptr, nullptr);
		_itemSet._dataPtr = new unsigned char*[threshold];

		_threshold = threshold;
	};


	NodeItemSet( int threshold , T *key , unsigned char *dataPtr , BTreeNode<T> *leftChildNode , BTreeNode<T> *rightChildNode ) : NodeItemSet(threshold)
	{
		_itemSet._keys[0] = key;
		_itemSet._dataPtr[0] = dataPtr;
		_itemSet._child[0]->first = leftChildNode;
		_itemSet._child[1]->first = rightChildNode;

	};



	int _threshold; // 閾値 子ノードの最大個数 
									// ( 閾値 - 1 ) キーバリューの最大個数

	int keyCnt( bool debugPrint = false );
	int childCnt( bool debugPrint = false );

	void insert( NodeItemSet *itemSet , BTreeNode<T> *parentNode = nullptr ); // 第二引数は親ノードを指定したい時に使う
	void set( NodeItemSet *fromitemSet , bool right );

	void pullRight( int index ); // indexに空きがくるように右シフトするj

};





template <typename T>
class BTreeNode{

private:

	int _threshold;  // 各ノードが保有できる子ノードの数
	
public:
	bool _isLeaf; // leafだったら探索を終了する 
	BTreeNode<T> *_parent;
	NodeItemSet<T> *_itemSet; // あとでprivateメンバーにする

	
	BTreeNode( int threshold ) : _isLeaf(true), _parent(nullptr) {

		_itemSet = new NodeItemSet<T>(threshold);

		_threshold = threshold;

	}

	BTreeNode<T>* find( T *key , BTreeNode **deepestNode );

	void regist( T *key , unsigned char *dataPtr , BTreeNode<T>* leftChildNode = nullptr , BTreeNode<T>* rightChildNode = nullptr );
	void add( NodeItemSet<T> *targetItemSet );
	void splitChild();

	static int bincmp( T *targetKey, T *baseKey , int tSize = 32 ); // あとで要修正
	static void printKey( T *targetKey , int tSize = 32 ); // あとで要修正
	// targetKey > baseKey -> return 1

	void refleshItemSet();

	// getter and setter
	bool isLeaf(){ return _isLeaf; }; // getter
	void isLeaf( bool ret ){ _isLeaf = ret; }; // setter
	int threshold(){ return _threshold; };
	void parent( BTreeNode<T> *parent ){ _parent = parent; };
	
};





template<typename T>
class BTree{

private:
	int _threshold;

public:

	BTreeNode<T> *_root;

	BTree( int threshold ){
		_root = new BTreeNode<T>(threshold);
	};

	static void printTree( BTreeNode<T>* targetNode );
	static void printNode( BTreeNode<T>* targetNode );

};






};


#endif // B1FC59BD_B0A1_4868_807D_4E4014B7B8D2 
