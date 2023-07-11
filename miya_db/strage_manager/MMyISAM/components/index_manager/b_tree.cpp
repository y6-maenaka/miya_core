#include "b_tree.h"



namespace miya_db{


template class BTreeNode<unsigned char>;
template struct NodeItemSet<unsigned char>;


/* 追加されるアイテムのフォーマットは以下に従うとする  特殊なメソッドなどは用意しない
 
	...  _itemSet - _keys[0]	 = key - _child[0] = leftChildNode
			- _child[1] = rightChildNode
			- _dataPtr[0] = value

*/
 



template <typename T>
int NodeItemSet<T>::keyCnt( bool debugPrint )
{
	int i = 0;
	while(  i <= _threshold - 1 &&  _itemSet._keys[i] != nullptr  )
	{ 
		i++;
	}

	if( debugPrint ){
		std::cout << " ================== KEY ================= " << "\n";
		int j = 0;
		while( j <= _threshold && _itemSet._keys[j] != nullptr  )
		{ 
			printf("- %p - (", _itemSet._keys[j] );
			for( int k=0; k<5; k++){
				printf("%c", _itemSet._keys[j][k]);
			}
			printf(")\n");
			j++;
		}
		std::cout << "\n";
	}
	
																																							 
	return i;
}




template <typename T>
int NodeItemSet<T>::childCnt( bool debugPrint )
{
	int i = 0;
	while( i <= _threshold &&  _itemSet._child[i]->first != nullptr  )
	{ 
		i++;
	}

	if( debugPrint ){
		std::cout << " ================== CHILD ================= " << "\n";
		int j = 0;
		while( j <= _threshold && _itemSet._child[j]->first != nullptr  )
		{ 
			printf("- %p - ", _itemSet._child[j]->first );
			j++;
		}
		std::cout << "\n";
	}
												
	return i;
}





template <typename T> // 子ノードからの繰り上がりも考慮したadd
void NodeItemSet<T>::insert( NodeItemSet *itemSet, BTreeNode<T> *parentNode )
{
	int i=0;
	while( i < keyCnt() && BTreeNode<T>::bincmp( itemSet->_itemSet._keys[0] , _itemSet._keys[i]) >= 0  ) // 値が同じになることはない
	{	
		i++;
	}

	/* 挿入したキーの位置に対応してセット内部の全ての配列をずらす */ // 最後尾だったら？
	pullRight( i );


	/* キーの挿入 */
	_itemSet._keys[i] = itemSet->_itemSet._keys[0];


	/* 子ノードの挿入 */ /* ※指定がある場合は 登録した子ノードの親ノードを設定 */
	
	if( itemSet->_itemSet._child[0]->first != nullptr  ){
		_itemSet._child[i] = itemSet->_itemSet._child[0];
		if( parentNode != nullptr ) _itemSet._child[i]->first->parent( parentNode );
	}

	if( itemSet->_itemSet._child[1]->first != nullptr ){
		_itemSet._child[i + 1] = itemSet->_itemSet._child[1];
		if( parentNode != nullptr ) _itemSet._child[i + 1]->first->parent( parentNode );
	}
	

	


	/* dataPtrの挿入 */
	_itemSet._dataPtr[i] = itemSet->_itemSet._dataPtr[0];

	return;
}


template <typename T>
void NodeItemSet<T>::set( NodeItemSet *fromItemSet , bool right )
{
	// ( threshold - 1 ) / 2

	int cnt = ( right && _threshold % 2 == 0 ) ? (((_threshold - 1) / 2) + 1) : ((_threshold - 1 ) / 2) ;

	std::cout << "cnt -> " << cnt << "\n";

	int startIdx = ( right ) ? ((_threshold / 2 )) : 0; // iに代入すると警告が出るため
	int i, j;

	for( i = startIdx , j = 0 ; j < cnt ; j++,  i++ )
	{
		printf(" Idx -> %d - %d \n", i , j );

		_itemSet._keys[j] = fromItemSet->_itemSet._keys[i];
		_itemSet._child[j] = fromItemSet->_itemSet._child[i];  
		_itemSet._dataPtr[j] = fromItemSet->_itemSet._dataPtr[i];
	}
	std::cout << "---------" << "\n";

	_itemSet._child[j] = fromItemSet->_itemSet._child[i]; // roop内でインクリメントされている
	
}



template <typename T>
void NodeItemSet<T>::pullRight( int index )
{

	if( index == (_threshold - 1) ) return; // 最後尾

	for( int i = _threshold - 1 ; i >= index + 1; i--){
		_itemSet._keys[i] = _itemSet._keys[i - 1];
	}

	return;
}




/*  ==========================================================================================  */








template<typename T>
BTreeNode<T>* BTreeNode<T>::find( T* key , BTreeNode** deepestNode ){

	int course = 1;

	*deepestNode = this;

	int i = 0;
	while( _itemSet->_itemSet._keys[i] != nullptr && i <= _threshold - 1 ){  // 2
	
		// 対象のキーは固定
		course = bincmp( key , _itemSet->_itemSet._keys[i] );

		if( course < 0 )
		{
			if( _itemSet->_itemSet._child[i]->first != nullptr )
				return _itemSet->_itemSet._child[i]->first->find( key , deepestNode ); 

			return nullptr;
		}

		else if( course == 0 ) // ヒットした場合
		{
			return this;
		}

		i++;
	}


	// key内に一致するものがなかった場合
	if( _isLeaf ) return nullptr;

	if( _itemSet->_itemSet._child[i]->first != nullptr ){
		return _itemSet->_itemSet._child[i]->first->find( key, deepestNode );
	}

	return nullptr;
}






template<typename T>
int BTreeNode<T>::bincmp( T *targetKey , T *baseKey , int tSize )
{
	
	if( targetKey == nullptr ) return 2;
	if( baseKey == nullptr ) return -2;

	unsigned char* cmpTargetKey = reinterpret_cast<unsigned char*>(targetKey);
	unsigned char* cmpBaseKey = reinterpret_cast<unsigned char*>(baseKey);

	unsigned char tmpTargetKey = 0x00, tmpBaseKey = 0x00;
	short ret;

	int i=0;
	while( i < tSize ){
		tmpTargetKey = ( i % 2 == 0 )  ? ((cmpTargetKey[i/2]) & 0xF0) >> 4 : cmpTargetKey[i/2] & 0x0F;
		tmpBaseKey = ( i % 2 == 0 )  ? ((cmpBaseKey[i/2]) & 0xF0) >> 4 : cmpBaseKey[i/2] & 0x0F;

		ret = static_cast<short>(tmpTargetKey) - static_cast<short>(tmpBaseKey);

		i++;
		if( ret == 0 ) continue;
		else if( ret > 0 ) return 1;	
		else return -1;
	}

	return 0;
}




template <typename T>
void BTreeNode<T>::printKey( T *targetKey , int tSize )
{
	unsigned char* keyBin = reinterpret_cast<unsigned char*>(targetKey);
	for( int i=0; i<tSize;i++){
		printf("%c", keyBin[i]);
	}
}






template <typename T>
void BTreeNode<T>::regist( T *key , unsigned char *dataPtr, BTreeNode<T>* leftChildNode, BTreeNode<T>* rightChildNode )
{

	BTreeNode<T> *deepestNode = nullptr;

	if( find( key , &deepestNode  ) != nullptr ){
		// update();
		std::cout << "key already registed" << "\n";
		return;
	}

	NodeItemSet<T> *targetItemSet = new NodeItemSet<T>( _threshold, key, dataPtr, leftChildNode, rightChildNode );	

	// 追加したい値がすでに登録されていなかった場合
	deepestNode->add( targetItemSet );

	return;
}





template <typename T>
void BTreeNode<T>::add( NodeItemSet<T> *targetItemSet )
{

	// rootだったらinsertしない


	_itemSet->insert( targetItemSet , this ); // 新規のキーバリューを追加する	// 予約スペースに挿入される可能性もあるj


	// 追加によって閾値を超えた場合
	if( _itemSet->keyCnt() >= _threshold )
	{	
		splitChild(); // 分割する


		if( _parent != nullptr ) {
			printf("added parednt prt -> %p\n", _parent );
			// ここで一時引き渡しようのitemSetを用意してNodeは削除するのもあり
			return this->_parent->add( this->_itemSet ); // ここでノード自体は不要になる
		}
		return;
	}

	return;
}



template <typename T>
void BTreeNode<T>::splitChild()
{

	std::cout << " !!== SPLIT CHILD NEEDED ==!!" << "\n";

	BTreeNode<T> *leftChildNode = new BTreeNode<T>(_threshold);  BTreeNode<T> *rightChildNode = new BTreeNode<T>(_threshold);

	/* 左ノードにパラメータの格納 */
	leftChildNode->_itemSet->set( _itemSet , false  );
	
	/* 右ノードにパラメータの格納 */
	rightChildNode->_itemSet->set( _itemSet , true );

	leftChildNode->_parent = this; rightChildNode->_parent = this; // 子ノードの親は自身を登録

	// 配列中央要素のKey & Value
	T *tmpKey = _itemSet->_itemSet._keys[ (_threshold /2 ) - ( 1 - (_threshold % 2 )) ];
	unsigned char *tmpDataPtr = _itemSet->_itemSet._dataPtr[ (_threshold /  2 ) - ( 1 - ( _threshold % 2 )) ]; 


	/* ItemSetを一旦クリアして中央値のみ移行する */
	refleshItemSet();
	_itemSet->_itemSet._keys[0] = tmpKey;
	_itemSet->_itemSet._dataPtr[0] = tmpDataPtr;


	/* 分割によって発生する迷子の親ノードの更新*/
	for( int i=0; i < leftChildNode->_itemSet->childCnt(); i++ ){
		leftChildNode->_itemSet->_itemSet._child[i]->first->parent( leftChildNode );
	}
	for( int i=0; i < rightChildNode->_itemSet->childCnt(); i++ ){
		rightChildNode->_itemSet->_itemSet._child[i]->first->parent( rightChildNode );
	}
	/* ========================================= */

	if( leftChildNode->_itemSet->childCnt() >= 1 ) leftChildNode->isLeaf(false);
	if( rightChildNode->_itemSet->childCnt() >= 2 ) rightChildNode->isLeaf(false);


	_itemSet->_itemSet._child[0]->first = leftChildNode;
	_itemSet->_itemSet._child[1]->first = rightChildNode; // 最後の要素で全ての配列要素が書き換わる


	this->_isLeaf = false; // splitChildした要素がリーフでない可能性もある

	// thisノードはポインタを変えずに分割する thisノードは中央値アイテムを引き継ぐ
	return;	
}





template <typename T>
void BTreeNode<T>::refleshItemSet()
{

	delete _itemSet;

	NodeItemSet<T> *newItemSet = new NodeItemSet<T>( _threshold );

	_itemSet =  newItemSet;

}









template <typename T>
void BTree<T>::printTree( BTreeNode<T>* targetNode )
{

	if( targetNode != nullptr ){
		printNode( targetNode );
	}

	if( !targetNode->isLeaf() ){
		for( int i=0 ; i<targetNode->_itemSet->childCnt() ; i++){
			std::cout << "\n\n==================================================" << "\n";
			std::cout << "child Idx [ " << i << " ] "; 
			std::cout << "node ptr [ " << targetNode->_itemSet->_itemSet._child[i]->first << " ]";
			printTree( targetNode->_itemSet->_itemSet._child[i]->first );
			std::cout << "==================================================\n\n" << "\n";
		}
	}

};


template <typename T>
void BTree<T>::printNode( BTreeNode<T> *targetNode )
{

	std::cout << "\n" << "-----------------------------------------------" << "\n";
	std::cout << "parent node prt -> " << targetNode->_parent  << "  | "<< "isLeaf[ "<< targetNode->isLeaf() <<" ]" << "\n"; 
	for( int i=0; i < targetNode->threshold(); i++){
		if( targetNode->_itemSet->_itemSet._keys[i] == nullptr ) std::cout << "nullptr | ";
		else{
			printf("(%p):", targetNode->_itemSet->_itemSet._keys[i]);
			BTreeNode<T>::printKey( targetNode->_itemSet->_itemSet._keys[i] , 5); 
			std::cout << " | ";
		}
	}	
	std::cout << "\n";
	std::cout << "child" << "\n";

	for( int i=0; i < targetNode->threshold() + 1; i++){
		if( targetNode->_itemSet->_itemSet._child[i]->first == nullptr ) std::cout << "nullptr | ";
		else{
			printf("(%p)", targetNode->_itemSet->_itemSet._child[i]->first);
			std::cout << " | ";
		}
	}
	std::cout << "\n" << "-----------------------------------------------" << "\n";

};





};
