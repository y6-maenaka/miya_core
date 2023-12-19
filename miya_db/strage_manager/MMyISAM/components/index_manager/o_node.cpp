#include "o_node.h"

#include "../page_table/optr_utils.h"

#include "../page_table/overlay_memory_manager.h"


namespace miya_db
{







void ViewItemSet::importItemSet( std::shared_ptr<OCItemSet> citemSet )
{
	std::array< std::shared_ptr<unsigned char>, DEFAULT_KEY_COUNT> *keySorce = citemSet->exportKeyArray(); // キーを一旦配列として書き出す
	std::copy( keySorce->begin() ,keySorce->end() , _key.begin() ); // 上書きで大丈夫

	std::array< std::shared_ptr<optr> , DEFAULT_DATA_OPTR_COUNT > *dataOptrSource = citemSet->exportDataOptrArray();
	std::copy( dataOptrSource->begin() , dataOptrSource->end(), _dataOPtr.begin() );

	std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT> *childSorce = citemSet->exportChildOptrArray(); // 子ノードを一旦配列として書き出す
	std::copy( childSorce->begin() ,childSorce->end(), _childOptr.begin() ); // 上書き

	return;
}


void ViewItemSet::moveInsertChildOptr( unsigned short index ,std::shared_ptr<optr> target )
{
	for( int i = _childOptr.size()-1; i > index; i-- )
		_childOptr.at(i) = _childOptr.at(i-1);

	_childOptr.at(index) = target;
}


void ViewItemSet::moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> target )
{
	for( int i = _dataOPtr.size()-1; i > index; i-- )
		_dataOPtr.at(i) = _dataOPtr.at(i-1);

	_dataOPtr.at(index) = target;
}



/*
	-------------------- 初期化関係 -----------------------
*/



ONodeItemSet::ONodeItemSet( std::shared_ptr<optr> __optr )
{
	//_optr = nullptr;
	_optr = __optr;
}


std::shared_ptr<optr> ONodeItemSet::Optr()
{
	return _optr;
}

void ONodeItemSet::clear()
{
	unsigned char zeroBuff[O_NODE_ITEMSET_SIZE]; memset( zeroBuff , 0x00 , sizeof(zeroBuff) );
	omemcpy( _optr.get(), zeroBuff, sizeof(zeroBuff) );
}


std::shared_ptr<optr> ONodeItemSet::parent() // 無限ループする可能性あり
{
	unsigned char oAddr[5];
	omemcpy( oAddr , _optr , 5 );

	return std::make_shared<optr>( oAddr , _optr->cacheTable() );
	//return _optr;
}


/*
  ---------------- キー操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::keyCount()
{
	return static_cast<unsigned short>( (*_optr + KEY_ELEMENT_OFFSET)->value() ); // itemSet先頭1バイトがそのままkeyCountになる
}

std::shared_ptr<unsigned char> ONodeItemSet::rawKey( unsigned short index )
{
	std::shared_ptr<optr> key = this->key( index );
	if( key == nullptr ) return nullptr;

	std::shared_ptr<unsigned char> ret = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
	omemcpy( ret.get() , key.get(), KEY_SIZE );
	return ret;
}

std::shared_ptr<optr> ONodeItemSet::key( unsigned short index ) // index : start with 0
{
	if( index > (keyCount()) ) return nullptr;

	return *_optr + ( KEY_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index * KEY_SIZE));
}

void ONodeItemSet::key( unsigned short index , std::shared_ptr<unsigned char> targetKey ) // キーの挿入
{
	if( index > DEFAULT_KEY_COUNT ) return;

	omemcpy( (*_optr + ( KEY_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index * KEY_SIZE))).get() , targetKey.get() , KEY_SIZE );
}


void ONodeItemSet::keyCount( unsigned short num ) // キー個数の挿入
{
	(*_optr + KEY_ELEMENT_OFFSET)->value( static_cast<unsigned char>(num) );
}


void ONodeItemSet::sortKey()
{
	// loke buble sort
	std::shared_ptr<optr> oKey_j0; std::shared_ptr<optr> oKey_j1;
	std::shared_ptr<unsigned char> key_j0 = std::shared_ptr<unsigned char>( new unsigned char[20] );
	std::shared_ptr<unsigned char> key_j1 = std::shared_ptr<unsigned char>( new unsigned char[20] );

	for( int i=0; i < keyCount()-1; i++ )
	{
		for( int j=keyCount()-1; j>i; j-- )
		{
			oKey_j0 = key(j);  oKey_j1 = key(j-1);
			omemcpy( key_j0.get(), oKey_j0, KEY_SIZE ); omemcpy( key_j1.get(), oKey_j1, KEY_SIZE );
			if( memcmp( key_j1.get() , key_j0.get() , KEY_SIZE ) > 0  )
			{
				omemcpy( oKey_j0.get() , key_j1.get() , KEY_SIZE );
				omemcpy( oKey_j1.get() , key_j0.get() , KEY_SIZE );
			}
		}
	}
}


void ONodeItemSet::moveInsertKey( unsigned short index , std::shared_ptr<unsigned char> targetKey )
{
	for( int i = keyCount(); i > index; i-- )
		key( i , rawKey(i-1) );

	key( index , targetKey  );
}



void ONodeItemSet::moveDeleteKey( unsigned short index )
{
	if( keyCount() <= 0 ) return;

	//std::shared_ptr<optr> keyOptr;
	for( int i=index; i<keyCount() - 1; i++ )
	{
		//std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		//keyOptr = key(i+1); omemcpy( rawKey.get() , keyOptr , KEY_SIZE );
		key( i ,  rawKey(i+1) );
	}

	keyCount( keyCount() - 1 );
}
/* -------------------------------------------------------------- */




/*
	---------------- 子ノード操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::childOptrCount()
{
	return static_cast<unsigned short>( (*_optr +( CHILD_ELEMENT_OFFSET ))->value() );
}

void ONodeItemSet::childOptrCount( unsigned short num )
{
	(*_optr + ( CHILD_ELEMENT_OFFSET ))->value( static_cast<unsigned char>(num)) ;
}


std::shared_ptr<optr> ONodeItemSet::childOptr( unsigned short index )
{
	// 指定インデックスのoptrを取得する
	if( index >= (childOptrCount()) ) return nullptr;

	std::shared_ptr<optr> childHead = *_optr + ( CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE) );

	unsigned char oAddr[5];
	omemcpy( oAddr , childHead , 5 );

	return std::make_shared<optr>( oAddr );

	//return childOptr;
}


void ONodeItemSet::childOptr( unsigned short index , std::shared_ptr<optr> targetOptr )
{
	if( index > DEFAULT_CHILD_COUNT ) return;
	if( targetOptr == nullptr )
	{
		unsigned char addrZero[5]; memset( addrZero, 0x00 , sizeof(addrZero) );
		omemcpy( (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , addrZero , NODE_OPTR_SIZE );
		return;
	}

	omemcpy( (*_optr + (CHILD_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , targetOptr->addr() , NODE_OPTR_SIZE );
}


void ONodeItemSet::moveInsertChildOptr( unsigned short index , std::shared_ptr<optr> targetONode )
{
	for( int i = childOptrCount(); i > index; i-- )
		childOptr(i, childOptr(i-1) );

	childOptr(index, targetONode );
}


void ONodeItemSet::moveDeleteChildOptr( unsigned short index )
{
	if( childOptrCount() <= 0 ) return;

	for( int i = index; i<childOptrCount()-1; i++ )
	{
		childOptr( i , childOptr(i+1) );
	}
	childOptrCount( childOptrCount() - 1 );
}


/* -------------------------------------------------------------- */




/*
	---------------- データオーバレイポインタ操作関係 -------------------------------------
*/
unsigned short ONodeItemSet::dataOptrCount()
{
	return static_cast<unsigned short>( (*_optr + DATA_OPTR_ELEMENT_OFFSET )->value() );
}

void ONodeItemSet::dataOptrCount( unsigned short num )
{
	(*_optr + ( DATA_OPTR_ELEMENT_OFFSET ))->value( static_cast<unsigned char>(num)) ;
}

std::shared_ptr<optr> ONodeItemSet::dataOptr( unsigned short index )
{
	if( index > (dataOptrCount()) ) return nullptr;

	std::shared_ptr<unsigned char> retOAddr = std::shared_ptr<unsigned char>( new unsigned char[DATA_OPTR_SIZE] );
	omemcpy( retOAddr.get(), *_optr + (( DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE) + (index*DATA_OPTR_SIZE)) , DATA_OPTR_SIZE );

	std::shared_ptr<optr> ret = std::make_shared<optr>( retOAddr.get() ); // そのままリターンでいい

	return ret;
}


void ONodeItemSet::dataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr )
{
	if( targetDataOptr == nullptr )
	{
		unsigned char addrZero[5]; memset( addrZero, 0x00 , sizeof(addrZero) );
		omemcpy( (*_optr + (DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , addrZero , NODE_OPTR_SIZE );
		return;
	}

	omemcpy( (*_optr + (DATA_OPTR_ELEMENT_OFFSET + ELEMENT_COUNT_SIZE + (index*NODE_OPTR_SIZE))).get() , targetDataOptr->addr() , NODE_OPTR_SIZE );
}


void ONodeItemSet::moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr )
{
	for( int i = dataOptrCount(); i > index; i-- )
		dataOptr( i , dataOptr(i-1) );

	dataOptr( index ,  targetDataOptr );
}

void ONodeItemSet::moveDeleteDataOptr( unsigned short index )
{
	if( dataOptrCount() <= 0 ) return;

	for( int i=index; i<dataOptrCount()-1; i++)
		dataOptr( i , dataOptr(i+1) );

	dataOptrCount( dataOptrCount() - 1 );
}


void ONodeItemSet::parent( std::shared_ptr<ONode> target )
{
	if( target == nullptr ) // 空の場合はnull(0x00)を挿入する
	{
		unsigned char addrZero[NODE_OPTR_SIZE]; memset( addrZero , 0x00 , NODE_OPTR_SIZE );
		omemcpy( _optr , addrZero , sizeof(addrZero) );
		return;
	}


	omemcpy( _optr , target->citemSet()->Optr()->addr() , NODE_OPTR_SIZE );

	return;
}


void ONodeItemSet::remove( unsigned short index )
{
	moveDeleteKey( index );
	moveDeleteDataOptr( index );
}
/* -------------------------------------------------------------- */


std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *ONodeItemSet::exportKeyArray()
{
	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *ret = new std::array< std::shared_ptr<unsigned char>  , DEFAULT_KEY_COUNT>;

	for( int i=0; i<keyCount(); i++)
		ret->at(i) = key(i)->mapToMemory( KEY_SIZE );

	return ret;
}


std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT> *ONodeItemSet::exportChildOptrArray()
{
	std::array< std::shared_ptr<optr>, DEFAULT_CHILD_COUNT> *ret = new std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT>;

	for( int i=0; i < childOptrCount(); i++ )
		ret->at(i) = childOptr(i);

	return ret;
}


std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *ONodeItemSet::exportDataOptrArray()
{
	std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *ret = new std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT >;

	for( int i=0; i< dataOptrCount(); i++ )
		ret->at(i) = dataOptr(i);

	return ret;
}











OItemSet::OItemSet( std::shared_ptr<ONodeItemSet> base )
{
	_base = base;
}
std::shared_ptr<ONodeItemSet> OItemSet::oNodeItemSet()
{
	return _base;
}
const std::shared_ptr<optr> OCItemSet::Optr(){
	return _base->Optr();
}
const std::shared_ptr<optr> OCItemSet::parent(){
	return _base->parent();
}
unsigned short OCItemSet::childOptrCount(){
	return _base->childOptrCount();
}
const std::shared_ptr<optr> OCItemSet::childOptr( unsigned short index ){
	return _base->childOptr(index);
}
unsigned short OCItemSet::keyCount(){
	return _base->keyCount();
}
const std::shared_ptr<optr> OCItemSet::key( unsigned short index ){
	return _base->key(index);
}
const std::shared_ptr<unsigned char> OCItemSet::rawKey( unsigned short index ){
	return _base->rawKey(index);
}
unsigned short OCItemSet::dataOptrCount(){
	return _base->dataOptrCount();
}
const std::shared_ptr<optr> OCItemSet::dataOptr( unsigned short index ){
	return _base->dataOptr(index);
}
std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *OCItemSet::exportKeyArray(){
	return _base->exportKeyArray();
}
std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT> *OCItemSet::exportChildOptrArray(){
	return _base->exportChildOptrArray();
}
std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *OCItemSet::exportDataOptrArray(){
	return _base->exportDataOptrArray();
}





OCItemSet::OCItemSet( std::shared_ptr<ONodeItemSet> base )
{
	_base = base;
}
std::shared_ptr<ONodeItemSet> OCItemSet::oNodeItemSet()
{
	return _base;
}
const std::shared_ptr<optr> OItemSet::Optr(){
	return _base->Optr();
}
void OItemSet::childOptr( unsigned short index , std::shared_ptr<optr> targetONode ){
	return _base->childOptr( index , targetONode );
}
void OItemSet::childOptrCount( unsigned short num ){
	return _base->childOptrCount( num );
}
void OItemSet::moveInsertChildOptr( unsigned short index , std::shared_ptr<optr> targetONode ){
	return _base->moveInsertChildOptr( index , targetONode );
}
void OItemSet::moveDeleteChildOptr( unsigned short index ){
	return _base->moveDeleteChildOptr(index);
}
void OItemSet::key( unsigned short index , std::shared_ptr<unsigned char> targetKey ){
	return _base->key(index, targetKey);
}
void OItemSet::keyCount( unsigned short num ){
	return _base->keyCount( num );
}
void OItemSet::sortKey(){
	return _base->sortKey();
}
void OItemSet::moveInsertKey( unsigned short index , std::shared_ptr<unsigned char> targetKey ){
	return _base->moveInsertKey(index, targetKey);
}
void OItemSet::moveDeleteKey( unsigned short index ){
	return _base->moveDeleteKey(index);
}
void OItemSet::dataOptrCount( unsigned short num ){
	return _base->dataOptrCount(num);
}
void OItemSet::dataOptr( unsigned short index ,std::shared_ptr<optr> targetDataOptr ){
	return _base->dataOptr( index , targetDataOptr );
}
void OItemSet::moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr ){
	return _base->moveInsertDataOptr( index , targetDataOptr );
}
void OItemSet::moveDeleteDataOptr( unsigned short index ){
	return _base->moveDeleteDataOptr(index);
}
void OItemSet::parent( std::shared_ptr<ONode> target ){
	return _base->parent( target );
}
void OItemSet::remove( unsigned short index ){
	return _base->remove(index);
}
void OItemSet::clear(){
	return _base->clear();
}












void ONode::itemSet( std::shared_ptr<ONodeItemSet> target )
{
	_itemSet = std::make_shared<ItemSet>( target );
}



ItemSet::ItemSet( std::shared_ptr<ONodeItemSet> target )
{
	_body = target;
	__itemSet = std::make_shared<OItemSet>( target );
	__citemSet = std::make_shared<OCItemSet>( target );
}


std::shared_ptr<OItemSet> ItemSet::itemSet()
{
	return __itemSet;
}

std::shared_ptr<OCItemSet> ItemSet::citemSet()
{
	return __citemSet;
}

std::shared_ptr<optr> ItemSet::baseOptr()
{
  return _body->Optr();
}




// ラップコンストラクタ
ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr ) // アドレスの引き継ぎ
{
	if( oMemoryManager == nullptr ) return;
	_oMemoryManager = oMemoryManager;

	if( baseOptr == nullptr ) // baseOptrが指定されていない場合は,itemSetを新規作成(割り当て)する
	{
		std::shared_ptr<optr> baseOptr = oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // ItemSet領域の確保
		this->itemSet( std::make_shared<ONodeItemSet>(baseOptr) ); // アイテムセットの新規作成と本メンバへセット
		this->itemSet()->clear(); // アイテムセットのクリア
	}
	else // baseOptrが指定されいる場合は,baseOptrのitemSetを使用する
	{
		this->itemSet( std::make_shared<ONodeItemSet>(baseOptr) );  // baseOptrはitemSetの開始点と同じ
	}
	return;
}

ONode::ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<ItemSet> itemSet )
{
  if( oMemoryManager == nullptr ) return;
  _oMemoryManager = oMemoryManager;

  if( itemSet == nullptr ) return;
  _itemSet = itemSet;
}


std::shared_ptr<OItemSet> ONode::itemSet()
{
	// std::cout << "\x1b[33m" << "ONode::itemSet()" << "\x1b[39m" << "\n";
	return _itemSet->itemSet();
}

std::shared_ptr<OCItemSet> ONode::citemSet()
{
	return _itemSet->citemSet();
}


std::shared_ptr<ONode> ONode::parent()
{
	//return std::make_shared<ONode>( new ONode( _itemSet->Optr()) );
	return std::make_shared<ONode>( _oMemoryManager , citemSet()->parent() );
}




std::shared_ptr<ONode> ONode::child( unsigned short index )
{
	std::shared_ptr<optr> retOptr = citemSet()->childOptr( index );
	if( retOptr == nullptr ) return nullptr;

	// ここでcacheTableを上書きしないとsafeファイルのoptｒを使用できない
	retOptr->cacheTable( _oMemoryManager->dataCacheTable() ); // _oMemoryManagerよりDatacacheTableを取得する
	return std::make_shared<ONode>( _oMemoryManager , retOptr );
}


void ONode::overlayMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_oMemoryManager = oMemoryManager;
}


std::shared_ptr<OverlayMemoryManager> ONode::overlayMemoryManager()
{
	return _oMemoryManager;
}



int ONode::findIndex( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
	std::shared_ptr<optr> keyOptr; int flag;
	for( int i=0; i<citemSet()->keyCount(); i++ ){
		keyOptr = citemSet()->key(i);  omemcpy( rawKey.get(), keyOptr, KEY_SIZE );
		flag = memcmp( targetKey.get() , rawKey.get(), KEY_SIZE );

		if( flag == 0 ) return i;
	}
	return -1;
}




std::shared_ptr<ONode> ONode::recursiveAdd( std::shared_ptr<unsigned char> targetKey, std::shared_ptr<optr> targetDataOptr, std::shared_ptr<ONode> targetONode )
{
	std::cout << "再帰追加が実行されます" << "\n";

	if( citemSet()->keyCount() >= DEFAULT_KEY_COUNT ) // ノードの分割が発生するパターン
	{
		auto splitONode = [&]( std::shared_ptr<unsigned char> *_targetKey , std::shared_ptr<optr> *_targetDataOptr  ,std::shared_ptr<ONode> *_targetONode ) // targetKeyとtargetONodeは入出力引数となる
		{
			std::cout << "ノード分割が発生します" << "\n";
			ViewItemSet viewItemSet; // 分裂中央のコントロールが面倒なので仮想的にサイズが一つ大きいアイテムセットを用意する
			viewItemSet.importItemSet( citemSet() ); // 仮想アイテムセットに既存のアイテムセット情報を取り込む

			// 一旦仮想アイテムセットに対象要素を追加する
			viewItemSet._key.at( DEFAULT_KEY_COUNT ) = *_targetKey; // 繰り越したキーの挿入
			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;
			// 論理アイテムセットのキーをソートする
			std::sort( viewItemSet._key.begin() , viewItemSet._key.end() , []( std::shared_ptr<unsigned char> a, std::shared_ptr<unsigned char> b){
				return memcmp( a.get() , b.get() , KEY_SIZE ) <= 0;
			});	 // viewItemSetのメソッドとした方が良い


			unsigned short keyInsertedIndex = 0; // 最終的にキーを挿入した位置を得る
			for( int i=0; i<viewItemSet._key.size(); i++ )
			{
				if( memcmp( viewItemSet._key.at(i).get(), targetKey.get(), KEY_SIZE ) == 0 ){
					keyInsertedIndex = i;
					break;
				}
			}

			viewItemSet.moveInsertDataOptr( keyInsertedIndex, *_targetDataOptr ); // dataOptrを挿入する

			if( *_targetONode != nullptr ){ // 追加対象がリーフノードでない限りは追加される
				viewItemSet.moveInsertChildOptr( keyInsertedIndex + 1 , (*_targetONode)->citemSet()->Optr() );
			}

			std::shared_ptr<unsigned char> separatorKey = viewItemSet._key.at( separatorKeyIndex ); // これは親ノードへの追加対象となる
			std::shared_ptr<optr> separatorDataOptr = viewItemSet._dataOPtr.at( separatorKeyIndex ); // これも親ノードへの追加対象となる
			// このアイテムセットは右子ノードとなる　分割対象の中央右ノードを格納する
			std::shared_ptr<ONode> splitONode = std::make_shared<ONode>( _oMemoryManager ); // (新たな右子ノード)  親ノードへの挿入対象となる 子ノードとして作成する


			// エラーでそう
			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	{ // キーの挿入
				splitONode->itemSet()->key( i , viewItemSet._key.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			for( int i=0; i<(DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1;i++)	{ // データポインタの挿入
				splitONode->itemSet()->dataOptr( i , viewItemSet._dataOPtr.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->dataOptrCount( (DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1 );

			splitONode->itemSet()->parent( this->parent() );

			// 本アイテムセットは左子ノードとなる 分割対象の中央左のキーを格納する
			for( int i=0; i<separatorKeyIndex; i++)
				itemSet()->key( i , viewItemSet._key.at(i) );
			itemSet()->keyCount(separatorKeyIndex);
			for( int i=0; i<separatorKeyIndex; i++) // データポインタの挿入
				itemSet()->dataOptr( i , viewItemSet._dataOPtr.at(i) );
			itemSet()->dataOptrCount(separatorKeyIndex);


			if( citemSet()->childOptrCount() > 0 ) // アイテムセットに子供ノードが存在する場合は分割したノードに分ける
			{
				int center = (viewItemSet._childOptr.size() % 2 == 0) ? (viewItemSet._childOptr.size()/2) : ((viewItemSet._childOptr.size()/2) );
				for( int i=0; i<center; i++ ){
					itemSet()->childOptr( i , viewItemSet._childOptr.at(i) ); // 分割後左ノード
				}
				itemSet()->childOptrCount( center );

				for( int i=center; i<viewItemSet._childOptr.size(); i++ ){
					splitONode->itemSet()->childOptr( i - center, viewItemSet._childOptr.at(i) );
					splitONode->itemSet()->childOptrCount(i-center+1);

					if( splitONode->citemSet()->childOptr( i - center ) != nullptr ){
						splitONode->child( i - center )->itemSet()->parent( splitONode ); // 分割後右ノード
					}
				}
				splitONode->itemSet()->childOptrCount( viewItemSet._childOptr.size() - center );
			}

			*_targetKey = separatorKey;
			*_targetDataOptr = separatorDataOptr;
			*_targetONode = splitONode; // 新たに作成したONode

		};

		splitONode( &targetKey , &targetDataOptr ,&targetONode );


		auto parentONode = this->parent();
		unsigned char oAddrZero[5]; memset( oAddrZero, 0x00 , sizeof(oAddrZero) );


		if( memcmp( parentONode->citemSet()->Optr()->addr(), oAddrZero, 5  )  == 0 )  // 自身がルートノード場合
		{
			//ONode* newRootNode = new ONode( _oMemoryManager ); // 新たなルートノードの作成
			auto newRootNode = std::shared_ptr<ONode>( new ONode(_oMemoryManager) );

			/* ルートノード情報の上書き */
			std::cout << "[ @ ] (Btree::ルートノードが上書きされました)" << "\n";
			std::shared_ptr<unsigned char> metaHeadOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] ); memset( metaHeadOAddr.get() , 0x00 , 5 );
			std::shared_ptr<optr> metaHeadOptr = std::shared_ptr<optr>( new optr(metaHeadOAddr.get() , _oMemoryManager->dataCacheTable()) );
			omemcpy( (*metaHeadOptr) + META_ROOT_NODE_OFFSET ,  newRootNode->citemSet()->Optr()->addr() , NODE_OPTR_SIZE );

			newRootNode->isLeaf( false );

			newRootNode->itemSet()->key( 0 ,targetKey );
			newRootNode->itemSet()->keyCount(1);

			newRootNode->itemSet()->dataOptr( 0 , targetDataOptr );
			newRootNode->itemSet()->dataOptrCount(1);

			std::cout << "アドレス:: "; shared_from_this()->citemSet()->Optr()->printAddr(); std::cout << "\n";

			newRootNode->itemSet()->childOptr( 0 , shared_from_this()->citemSet()->Optr() ); // 本Oノードの追加
			newRootNode->itemSet()->childOptrCount( 1 );

			std::cout << "ストアドアドレス :: "; newRootNode->citemSet()->childOptr(0)->printAddr(); std::cout << "\n";

			newRootNode->itemSet()->childOptr( 1 ,targetONode->citemSet()->Optr() ); // 新規に作成したノードの追加
			newRootNode->itemSet()->childOptrCount(2);

			this->itemSet()->parent( newRootNode ); // 左ONodeの親はこれにセット
			targetONode->itemSet()->parent( newRootNode ); // 右ONOdeの親はこれにセット
			newRootNode->itemSet()->parent( nullptr ); // 新たなルートノードの親ノードは0x00(無設定)になる

			return newRootNode;  // 必ずリターンする
		}

		// 親ノードが存在する場合
		return parentONode->recursiveAdd( targetKey, targetDataOptr ,targetONode );
	}

	else{ // 単純追加

		itemSet()->key( citemSet()->keyCount() , targetKey );
		itemSet()->keyCount( citemSet()->keyCount() + 1 );
		itemSet()->sortKey();


		unsigned short keyInsertedIndex = 0; // キー素挿入した位置を得る
		std::shared_ptr<optr> oKey; std::shared_ptr<unsigned char> uKey = std::shared_ptr<unsigned char>( new unsigned char[20] );
		for( int i=0; i<citemSet()->keyCount(); i++ )
		{
			oKey = citemSet()->key(i); omemcpy( uKey.get(), oKey , KEY_SIZE );
			if( memcmp( uKey.get(), targetKey.get(), KEY_SIZE ) == 0 ){
				keyInsertedIndex = i;
				break;
			}
		}
		itemSet()->moveInsertDataOptr( keyInsertedIndex , targetDataOptr );
		itemSet()->dataOptrCount( citemSet()->dataOptrCount() + 1 );

		if( targetONode != nullptr )
		{
			itemSet()->moveInsertChildOptr( keyInsertedIndex + 1  , targetONode->citemSet()->Optr() );
			itemSet()->childOptrCount( citemSet()->keyCount() + 1 );
		}


		return nullptr; // 一応
	}

	return nullptr;
}



std::shared_ptr<ONode> ONode::remove( std::shared_ptr<unsigned char> targetKey )
{
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	int index = findIndex( targetKey );
	if( index < 0 ) return nullptr;

	if( citemSet()->childOptrCount() >= 1 ) // 削除対象が中間ノードであった場合
	{
		std::cout << "中間ノード削除" << "\n";
		// 対象ノードの左サブツリーから最大値(ツリー内で対象の次に大きい)を取得する
		auto subtreeMax = child(0)->subtreeMax();
		itemSet()->key( index , subtreeMax->citemSet()->rawKey( subtreeMax->citemSet()->keyCount() - 1 ) ); // 削除対象にサブツリーマックス要素を追加
		itemSet()->dataOptr( index , subtreeMax->citemSet()->dataOptr( subtreeMax->citemSet()->dataOptrCount() - 1 ) );  // 削除対象にサブツリーマックス要素を追加

		auto candidateNewRootONode = subtreeMax->remove( subtreeMax->citemSet()->rawKey( subtreeMax->citemSet()->keyCount() - 1 ) );

		if  // ルートノードが書き換わり, 子ノードのみ直列に連結している場合
		( // 無理矢理感がある
			candidateNewRootONode != nullptr &&
			candidateNewRootONode->citemSet()->keyCount() <= 0 &&
			candidateNewRootONode->citemSet()->childOptrCount() == 1 &&
			memcmp( candidateNewRootONode->parent()->citemSet()->Optr()->addr() , addrZero , sizeof(addrZero) ) == 0
		)
		{
			candidateNewRootONode->child(0)->itemSet()->parent(nullptr);
			return candidateNewRootONode->child(0);
		}

		return candidateNewRootONode;
	}

	// 本ノードがルートノードの場合は単純削除し,ルートノードは変更しない
	if( citemSet()->keyCount() > 1 || memcmp( parent()->citemSet()->Optr()->addr() , addrZero, sizeof(addrZero) ) == 0 )  // 単純削除のケース
	{
		itemSet()->remove( index );
		return nullptr;
	}

	// 本ノードがルートノードで要素が1つの場合,アイテムセットを空にしてルートノードは変更しない
	if( memcmp( this->parent()->citemSet()->Optr()->addr(), addrZero , sizeof(addrZero)) == 0 )
	{
		itemSet()->moveDeleteKey(0);
		return nullptr;
	}

	return this->parent()->underflow( shared_from_this() ); // アンダーフロー発生を通知

	/*
		削除パターン

		[ 削除対象がリーフノードの存在 ]
			1. 削除対象が属するONodeが要素を2つ以上(削除対象を含む)持っている場合
			   -> 単純削除
			2. 削除対象が属するONodeが要素が1つ以下(削除対象のみ)の場合 && 兄妹ノードから持って来れる要素がある

			3. 削除対象が属するONodeが要素が1つ以下(削除対象のみ)の場合 && 兄妹ノードから持って来れる要素がない



		[ 削除対処がリーフ以外に存在 ]
	*/

}



// 通知元の子ノードを通知するべきか？
std::shared_ptr<ONode> ONode::underflow( std::shared_ptr<ONode> sourceONode )
{
	std::cout << "\x1b[33m" << "underflowが発生しました" << "\x1b[39m" << "\n";
	// とりあえずsourceONodeを元に,削除対象のONodeのインデックスを逆引きする(非効率)
	int index = -1;
	for( int i=0; i<=citemSet()->childOptrCount() - 1; i++ ){
		index = ( memcmp( sourceONode->citemSet()->Optr()->addr(), child(i)->citemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) return nullptr;


	/* キー移動 */
	// 左右兄弟を検索してキー移動が可能であれば移動する
	// 検索対象は対象と隣接している兄弟(対象からインデックス差が±1)ONodeだけ　
	auto targetONode = child(index);
	auto leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	auto rightChildONode = this->child(index+1);



	// 兄弟ノードから移動できるキーが存在するか確認する
	if( leftChildONode != nullptr && leftChildONode->citemSet()->keyCount() >= 2 ){
		std::cout << "左兄弟ノードからキーを移動" << "\n";
		sourceONode->itemSet()->key( 0 , citemSet()->rawKey(index-1) );
		sourceONode->itemSet()->keyCount(1);
		sourceONode->itemSet()->dataOptr( 0 , citemSet()->dataOptr(index-1) );
		sourceONode->itemSet()->dataOptrCount(1);

		itemSet()->key( index-1, leftChildONode->citemSet()->rawKey( leftChildONode->citemSet()->keyCount() - 1 ) ); // 最後尾を取得
		leftChildONode->itemSet()->moveDeleteKey( leftChildONode->citemSet()->keyCount() - 1 );
		itemSet()->dataOptr( index-1 , leftChildONode->citemSet()->dataOptr(leftChildONode->citemSet()->dataOptrCount() - 1 ) );
		leftChildONode->itemSet()->moveDeleteDataOptr( leftChildONode->citemSet()->dataOptrCount() - 1 );
		return nullptr;
	}
	if( rightChildONode != nullptr && rightChildONode->citemSet()->keyCount() >= 2 )
	{
		std::cout << "右兄弟ノードからキーを移動" << "\n";
		// 右ノードからキー移動
		sourceONode->itemSet()->key( 0 , citemSet()->rawKey(index) );
		sourceONode->itemSet()->keyCount(1);
		sourceONode->itemSet()->dataOptr( 0 , citemSet()->dataOptr(index) );
		sourceONode->itemSet()->dataOptrCount(1);

		itemSet()->key( index, rightChildONode->citemSet()->rawKey( 0 ) ); // 最後尾を取得
		rightChildONode->itemSet()->moveDeleteKey( 0 );
		itemSet()->dataOptr( index , rightChildONode->citemSet()->dataOptr(0) );
		rightChildONode->itemSet()->moveDeleteDataOptr(0);

		return nullptr;
	}

	// underflowによるキー移動ができなかったので,マージ処理を行う
	/* マージ */
	return merge( index );
}



std::shared_ptr<ONode> ONode::merge( std::shared_ptr<ONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=citemSet()->childOptrCount() - 1; i++ ){
		index = ( memcmp( sourceONode->citemSet()->Optr()->addr(), child(i)->citemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) nullptr;
	return this->merge( index );
}


std::shared_ptr<ONode> ONode::merge( unsigned short index ) // mergeが呼び出される時,兄弟ノードの各要素数は1
{
	// マージ前処理
	// 子ノードに追加して本ノードを削除対象とする
	std::cout << "\x1b[33m" << "underflow(マージ)が発生しました" << "\x1b[39m" << "\n";
	if( index == -1 ) return nullptr;

	auto targetONode = child(index);
	auto leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	auto rightChildONode = this->child(index+1);

	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );

	// マージの種類
	// 1. 兄弟ノードに要素が2つ以上ある場合
	if( citemSet()->keyCount() >= 2 )
	{
		// indexの箇所の子ノードを解消する
		if( leftChildONode != nullptr )
		{
			std::cout << "左兄弟ノードマージ" << "\n";
			itemSet()->moveDeleteChildOptr( index );

			leftChildONode->itemSet()->key( leftChildONode->citemSet()->keyCount() ,citemSet()->rawKey(index-1) );
			leftChildONode->itemSet()->keyCount( leftChildONode->citemSet()->keyCount() + 1 );
			itemSet()->moveDeleteKey(index-1);

			leftChildONode->itemSet()->dataOptr( leftChildONode->citemSet()->dataOptrCount(), citemSet()->dataOptr(index-1) );
			leftChildONode->itemSet()->dataOptrCount( leftChildONode->citemSet()->dataOptrCount() + 1 );

			//_itemSet->moveInsertChild( _itemSet->childCount(), leftChildONode->itemSet()->child( leftChildONode->itemSet()->childCount()) );
			//_itemSet->childCount( _itemSet->childCount() + 1 );
			//leftChildONode->itemSet()->moveDeleteChild( leftChildONode->itemSet()->childCount() );
			return nullptr; //　ルートノードを変更することなく終了
		}

		if( rightChildONode != nullptr )
		{
			std::cout << "右兄弟ノードマージ前処理" << "\n";
			itemSet()->moveDeleteChildOptr( index );

			rightChildONode->itemSet()->moveInsertKey( 0 , citemSet()->rawKey(index) );
			rightChildONode->itemSet()->keyCount( rightChildONode->citemSet()->keyCount() + 1 );
			itemSet()->moveDeleteKey(index);

			rightChildONode->itemSet()->moveInsertDataOptr( 0 , citemSet()->dataOptr(index) );
			rightChildONode->itemSet()->dataOptrCount( rightChildONode->citemSet()->dataOptrCount() );

			//_itemSet->moveInsertChild( _itemSet->childCount() , rightChildONode->itemSet()->child(0) );
			//_itemSet->childCount( _itemSet->childCount() + 1 );
			//rightChildONode->itemSet()->moveDeleteChild(0);
			return nullptr;
		}
	}


	if( leftChildONode != nullptr )
	{
		/* 左再帰マージ前処理
			1. 左ノードに親ノードの要素を移動させる
			2. 親ノードの要素を削除する
			3. 親ノード(中身は空)に対してマージ処理を行う
		*/
		std::cout << "左再帰マージ前処理" << "\n";
		leftChildONode->itemSet()->moveInsertKey( leftChildONode->citemSet()->keyCount() , citemSet()->rawKey(index-1) ); // 0でもいい
		leftChildONode->itemSet()->keyCount( leftChildONode->citemSet()->keyCount() + 1);
		itemSet()->moveDeleteKey(index-1);

		leftChildONode->itemSet()->moveInsertDataOptr( leftChildONode->citemSet()->dataOptrCount() , citemSet()->dataOptr(index-1) );
		leftChildONode->itemSet()->dataOptrCount( leftChildONode->citemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index-1);

		itemSet()->moveDeleteChildOptr(index); // 子ノードの削除

		if( memcmp( parent()->citemSet()->Optr()->addr(), addrZero, sizeof(addrZero)  ) == 0 ) {
			child(0)->itemSet()->parent(nullptr);
			return child(0);
		}
		//leftChildONode->itemSet()->moveInsertChild( leftChildONode->itemSet()->childCount(), itemSet()->child(index) );
		//leftChildONode->itemSet()->childCount( leftChildONode->itemSet()->childCount() + 1 );
		//leftChildONode->itemSet()->child( leftChildONode->itemSet()->childCount() - 1)->parent( leftChildONode );

		return this->parent()->recursiveMerge( shared_from_this() );
	}

	if( rightChildONode != nullptr )
	{
		std::cout << "右再帰マージ前処理" << "\n";

		rightChildONode->itemSet()->moveInsertKey( 0 , citemSet()->rawKey(index) ); // 0でもいい？
		rightChildONode->itemSet()->keyCount( rightChildONode->citemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index);

		rightChildONode->itemSet()->moveInsertDataOptr( 0 , citemSet()->dataOptr(index) );
		rightChildONode->itemSet()->dataOptrCount( rightChildONode->citemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index);

		itemSet()->moveDeleteChildOptr(index);
		// ここで本ノードは空になっているはず

		if( memcmp( parent()->citemSet()->Optr()->addr(), addrZero, sizeof(addrZero)  ) == 0 ){
			child(0)->itemSet()->parent(nullptr);
			return child(0);
		}
		//rightChildONode->itemSet()->moveInsertChild( 0 , itemSet()->child(index) );
		//rightChildONode->itemSet()->childCount( rightChildONode->itemSet()->childCount() + 1 );
		//rightChildONode->itemSet()->child(0)->parent( rightChildONode );
		//itemSet()->moveDeleteChild(index);

		return this->parent()->recursiveMerge( shared_from_this() );
	}

	return nullptr;
}



std::shared_ptr<ONode> ONode::recursiveMerge( std::shared_ptr<ONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=citemSet()->childOptrCount() - 1; i++ ){
		index = ( memcmp( sourceONode->citemSet()->Optr()->addr(), child(i)->citemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) nullptr;
	return this->recursiveMerge( index );
}




// これが呼び出される時は必ず親の要素(キー)数が1こである
std::shared_ptr<ONode> ONode::recursiveMerge( unsigned short index ) // mergeが呼び出されて後に必ず呼び出される
{
	std::cout << "再帰マージが実行されます" << "\n";
	if( citemSet()->keyCount() <= 0 ){
		std::cout << "親ノードを削除します" << "\n";
		child(0)->itemSet()->parent(nullptr);


		std::cout << "@@@ "; child(0)->citemSet()->parent()->printValueContinuously(5); std::cout << "\n";
		return child(0); // 新たなルートノードを返却する
	}

	if( index == -1 ) return nullptr;

	auto targetONode = child(index);
	auto leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	auto rightChildONode = this->child(index+1);


	if( leftChildONode != nullptr && leftChildONode->citemSet()->keyCount() >= 2 ) // 兄弟左ノードから親ノード要素を押し出す
	{
		std::cout << "左移動再帰マージ" << "\n";
		// 再帰追加の際はindex要素はまだ死んでいない
		targetONode->itemSet()->key( 0 , citemSet()->rawKey(0) );
		targetONode->itemSet()->keyCount( targetONode->citemSet()->keyCount() + 1 );
		itemSet()->keyCount(0);
		targetONode->itemSet()->dataOptr( 0 , citemSet()->dataOptr(0) );
		itemSet()->dataOptrCount(0);

		itemSet()->key( 0 , leftChildONode->citemSet()->rawKey(leftChildONode->citemSet()->keyCount()) );  // 最後尾
		itemSet()->keyCount(1);
		leftChildONode->itemSet()->moveDeleteKey( leftChildONode->citemSet()->keyCount() );
		itemSet()->dataOptr( 0 , leftChildONode->citemSet()->dataOptr(leftChildONode->citemSet()->dataOptrCount()) );
		itemSet()->dataOptrCount(1);
		leftChildONode->itemSet()->moveDeleteDataOptr(leftChildONode->citemSet()->dataOptrCount());

		targetONode->itemSet()->moveInsertChildOptr( 0, leftChildONode->child(leftChildONode->citemSet()->childOptrCount())->citemSet()->Optr() );
		targetONode->itemSet()->childOptrCount( targetONode->citemSet()->childOptrCount() + 1 );
		targetONode->child( 0 )->itemSet()->parent( targetONode );
		//printf("%p\n", targetONode->itemSet()->child( targetONode->itemSet()->childCount()).get() );
		leftChildONode->itemSet()->moveDeleteChildOptr( leftChildONode->citemSet()->childOptrCount() );
		return nullptr;

	}

	if( rightChildONode != nullptr && rightChildONode->citemSet()->keyCount() >= 2 ) // 兄弟右ノードから親ノードの要素を押し出す
	{
		std::cout << "右移動再帰マージ" << "\n";
		// 再帰追加の際はindex要素はまだ死んでいない
		targetONode->itemSet()->key( 0 , citemSet()->rawKey(0) );
		targetONode->itemSet()->keyCount( targetONode->citemSet()->keyCount() + 1 );
		itemSet()->keyCount(0);
		targetONode->itemSet()->dataOptr( 0 , citemSet()->dataOptr(0) );
		itemSet()->dataOptrCount(0);

		itemSet()->key( 0 , rightChildONode->citemSet()->rawKey(0) );
		itemSet()->keyCount(1);
		rightChildONode->itemSet()->moveDeleteKey(0);
		itemSet()->dataOptr( 0 , rightChildONode->citemSet()->dataOptr(0) );
		itemSet()->dataOptrCount(1);
		rightChildONode->itemSet()->moveDeleteDataOptr(0);

		targetONode->itemSet()->moveInsertChildOptr( targetONode->citemSet()->childOptrCount(), rightChildONode->child(0)->citemSet()->Optr() );
		targetONode->itemSet()->childOptrCount( targetONode->citemSet()->childOptrCount() + 1 );
		targetONode->child( targetONode->citemSet()->childOptrCount() - 1 )->itemSet()->parent( targetONode );
		//printf("%p\n", targetONode->itemSet()->child( targetONode->itemSet()->childCount()).get() );
		rightChildONode->itemSet()->moveDeleteChildOptr(0);
		return nullptr;
	}



	// 最終手段
	if( leftChildONode != nullptr )
	{
		std::cout << "トップレベル左再帰マージ" << "\n";
		leftChildONode->itemSet()->moveInsertKey( leftChildONode->citemSet()->keyCount(), citemSet()->rawKey(index-1) );
		leftChildONode->itemSet()->keyCount( leftChildONode->citemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index-1);

		leftChildONode->itemSet()->moveInsertDataOptr( leftChildONode->citemSet()->dataOptrCount() , citemSet()->dataOptr(index-1) );
		leftChildONode->itemSet()->dataOptrCount( leftChildONode->citemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index-1);

		leftChildONode->itemSet()->moveInsertChildOptr( leftChildONode->citemSet()->childOptrCount() , targetONode->child(0)->citemSet()->Optr() );
		leftChildONode->itemSet()->childOptrCount( leftChildONode->citemSet()->childOptrCount() + 1 );
		leftChildONode->child( leftChildONode->citemSet()->childOptrCount() - 1 )->itemSet()->parent( leftChildONode );

		itemSet()->moveDeleteChildOptr(index);


		if( citemSet()->keyCount() <= 0 ){
			child(0)->itemSet()->parent(nullptr);
			return child(0); // ルートノード変更
		}
		return nullptr; // ルートノードの変更は発生しない

	}

	if( rightChildONode != nullptr )
	{
		std::cout << "トップレベル右再帰マージ" << "\n";
		rightChildONode->itemSet()->moveInsertKey( 0 , citemSet()->rawKey(index) ); // 0でもいい？
		rightChildONode->itemSet()->keyCount( rightChildONode->citemSet()->keyCount() + 1 );
		itemSet()->moveDeleteKey(index);

		rightChildONode->itemSet()->moveInsertDataOptr( 0 , citemSet()->dataOptr(index) );
		rightChildONode->itemSet()->dataOptrCount( rightChildONode->citemSet()->dataOptrCount() + 1 );
		itemSet()->moveDeleteDataOptr(index);
		// ここで本ノードは空になっているはず

		rightChildONode->itemSet()->moveInsertChildOptr( 0 , targetONode->child(0)->citemSet()->Optr() );
		rightChildONode->itemSet()->childOptrCount( rightChildONode->citemSet()->childOptrCount() + 1 );
		rightChildONode->child( rightChildONode->citemSet()->childOptrCount() - 1 )->itemSet()->parent( rightChildONode );

		itemSet()->moveDeleteChildOptr(index);

		if( citemSet()->keyCount() <= 0 ){
			child(0)->itemSet()->parent(nullptr);
			return child(0);
		}
		return nullptr;
	}

	return nullptr;
}




std::shared_ptr<ONode> ONode::subtreeMax()
{
	if( citemSet()->childOptrCount() >= 1 ) // 本ノードに子ノードが存在すれば,最大の子ノードを再帰的に探索する
	 return child( citemSet()->childOptrCount() - 1 )->subtreeMax();

	return shared_from_this();
}






std::shared_ptr<ONode> ONode::subtreeONodeFind( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<ONode> candidateChild;
	std::shared_ptr<optr> keyOptr;
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
	int flag;

	for( int i=0; i<citemSet()->keyCount(); i++ )
	{
		rawKey = citemSet()->rawKey(i);
		flag = memcmp( targetKey.get() , rawKey.get() , KEY_SIZE );

		if( flag < 0 ){
			candidateChild = child(i);
			if( candidateChild == nullptr ) return nullptr;
			goto direct;
		}
		else if( flag == 0 ){
			return shared_from_this();
		}
	}

	candidateChild = child( citemSet()->childOptrCount() -1 );
	if( candidateChild == nullptr ) return nullptr;

	direct:
		return candidateChild->subtreeONodeFind( targetKey );
}


std::shared_ptr<optr> ONode::subtreeFind( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<ONode> candidateChild;

	std::shared_ptr<optr> keyOptr;
	std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[5] );
	int flag;

	for( int i=0; i<citemSet()->keyCount(); i++ )
	{
		keyOptr = citemSet()->key(i); omemcpy( rawKey.get(), keyOptr , 5 );
		flag = memcmp( targetKey.get() , rawKey.get() , 5 );


		if( flag < 0 ){
			candidateChild = child(i);
			if( candidateChild == nullptr ) return nullptr;
			goto direct;
		}
		else if( flag == 0 ){
			return citemSet()->dataOptr(i);
		}
	}


	candidateChild = child( citemSet()->childOptrCount() -1 );
	if( candidateChild == nullptr ) return nullptr;

	direct:
		return candidateChild->subtreeFind( targetKey );
}


std::shared_ptr<optr> ONode::baseOptr()
{
  return _itemSet->baseOptr();
}

















};
