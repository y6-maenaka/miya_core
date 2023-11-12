#include "safe_o_node.h"

#include "./safe_index_manager.h"
#include "../../components/page_table/optr_utils.h"

namespace miya_db
{




ONodeConversionTable::ONodeConversionTable( std::shared_ptr<OverlayMemoryManager> safeOMemoryManager )
{
    _safeOMemoryManager = safeOMemoryManager;
}


void ONodeConversionTable::init()
{
    _entryMap.clear();
}


std::pair< std::shared_ptr<SafeONode> , bool > ONodeConversionTable::ref( std::shared_ptr<optr> target )
{
	optr* targetOptr = target.get();
    auto itr = _entryMap.find( *targetOptr );
    if( itr == _entryMap.end() ){
		//std::cout << "マッピング情報がありません" << "\n";
		std::shared_ptr<optr> retOptr = target;
		retOptr->cacheTable( _normalOMemoryManager->dataCacheTable() );
		return std::make_pair( std::make_shared<SafeONode>( _normalOMemoryManager , retOptr ), false );
	} 

	//std::cout << "\x1b[33m" << "マッピング情報が存在します" << "\x1b[39m" << "\n";
	std::shared_ptr<optr> retOptr = std::make_shared<optr>(itr->second);
	retOptr->cacheTable( _safeOMemoryManager->dataCacheTable() );
  	return std::make_pair( std::make_shared<SafeONode>( _safeOMemoryManager ,retOptr ), true ); // 変換テーブルに要素が存在した場合
}


void ONodeConversionTable::regist( std::shared_ptr<optr> key , std::shared_ptr<optr> value )
{
    // コピーを作成する
    // _entryMap[key] = value;
	optr *_key = key.get(); optr *_value = value.get();
	_entryMap.insert( { *_key , *_value } );


	std::cout << "\x1b[31m";
	std::cout << "[ - ] エントリが作成されました" << "\n";
	std::cout << "  :: (key) :: "; key->printAddr(); std::cout << "\n";
	std::cout << "  :: (value) :: "; value->printAddr(); std::cout << "\n";
	std::cout << "\x1b[39m";
	
  // ここでONodeとマッピングされるSafeONodeはメンバの optrとoMemoryManager がsafeModeのものになっている
	
}

void ONodeConversionTable::safeOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
    _safeOMemoryManager = oMemoryManager;
}


const std::shared_ptr<OverlayMemoryManager> ONodeConversionTable::safeOMemoryManager()
{
    return _safeOMemoryManager;
}


void ONodeConversionTable::normalOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
	_normalOMemoryManager = oMemoryManager;
}

const std::shared_ptr<OverlayMemoryManager> ONodeConversionTable::normalOMemoryManager()
{
	return _normalOMemoryManager;
}


void ONodeConversionTable::printEntryMap()
{
	int i=0;
	for( auto itr : _entryMap )
	{
		std::cout << "[ Entry ("<< i << ")]" << "\n";
		if( ((100 + itr.first.offset()) % O_NODE_ITEMSET_SIZE) == 0 ) std::cout << "\x1b[33m";
		if( ((100 + itr.first.offset() + SAFE_MODE_COLLICION_OFFSET) % O_NODE_ITEMSET_SIZE) == 0 ) std::cout << "\x1b[35m";
		std::cout << " -key :: "; itr.first.printAddr(); std::cout << "\n";
		std::cout << "\x1b[39m";

		if( ((100 + itr.second.offset()) % O_NODE_ITEMSET_SIZE) == 0 ) std::cout << "\x1b[33m";
		if( ((100 + itr.second.offset() + SAFE_MODE_COLLICION_OFFSET) % O_NODE_ITEMSET_SIZE) == 0 ) std::cout << "\x1b[35m";
		std::cout << " -value :: "; itr.second.printAddr(); std::cout << "\n";
		std::cout << "\x1b[39m";
		std::cout << "\n";
		i++;
	}
	return ;
}










SafeONode::SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager ) : ONode( nullptr ) // nullptrを渡して新規に割り当てさせないようにする
{ 
    // セーフモードファイルのメモリマネージャーをセット
	if( oMemoryManager == nullptr )	
		_oMemoryManager = _conversionTable.safeOMemoryManager();
	else
		_oMemoryManager = oMemoryManager;

	// SafeONodeを新規作成なので,ONodeファイルからコピーする必要はない
	std::shared_ptr<optr> baseOptr = _oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // 新規作成の場合
	ONode::itemSet( std::make_shared<ONodeItemSet>(baseOptr) );
	ONode::itemSet()->clear(); // ゼロ埋め

	_conversionTable.regist( baseOptr  , baseOptr );
    std::cout << "\x1b[35m" << "SafeONodeが初期化されました" << "\x1b[39m" << "\n";
}



SafeONode::SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr ) : ONode( oMemoryManager , baseOptr ) 
{
    return;
}


std::shared_ptr<SafeONode> SafeONode::parent()
{
   return std::make_shared<SafeONode>( *(static_cast<SafeONode*>(ONode::parent().get())) );
}



std::shared_ptr<SafeONode> SafeONode::child( unsigned short index )
{
	std::shared_ptr<ONode> retONode = ONode::child(index);
	if( retONode == nullptr ) return nullptr;
	return std::make_shared<SafeONode>( *(static_cast<SafeONode*>(retONode.get())) );
}


std::shared_ptr<OCItemSet> SafeONode::citemSet() 
{
	std::shared_ptr<SafeONode> convertedONode = (_conversionTable.ref( shared_from_this()->ONode::citemSet()->Optr() )).first;
	
	//std::cout << "変換情報を取得しました" << "\n";

	return convertedONode->ONode::citemSet();
}


std::shared_ptr<OItemSet> SafeONode::itemSet() 
{
	std::pair< std::shared_ptr<SafeONode>, bool > refRet = _conversionTable.ref( shared_from_this()->ONode::citemSet()->Optr() );
	std::shared_ptr<SafeONode> convertedONode = refRet.first;

	if( !(refRet.second) ) // 変換テーブルエントリが作成されていない場合 -> コピーを作成してエントリを追加する
	{ 
		std::shared_ptr<optr> baseOptr = _conversionTable.safeOMemoryManager()->allocate( O_NODE_ITEMSET_SIZE ); // Safeファイルにインデックスを作成する
		omemcpy( baseOptr.get() ,  convertedONode->ONode::citemSet()->Optr().get() , O_NODE_ITEMSET_SIZE ); // コピーを作成する
		std::shared_ptr<SafeONode> newSafeONode = std::make_shared<SafeONode>( _conversionTable.safeOMemoryManager(), baseOptr ); // 新たにSafeNodeとして作成する

		_conversionTable.regist( baseOptr , baseOptr );
		_conversionTable.regist( shared_from_this()->ONode::citemSet()->Optr() , baseOptr );

		convertedONode = newSafeONode;
	}

	return convertedONode->ONode::itemSet();
}


std::shared_ptr<SafeONode> SafeONode::subtreeMax()
{
	if( citemSet()->childOptrCount() >= 1 ) 
	 return child( citemSet()->childOptrCount() - 1 )->subtreeMax();

	return shared_from_this();
}





std::shared_ptr<SafeONode> SafeONode::recursiveAdd( std::shared_ptr<unsigned char> targetKey, std::shared_ptr<optr> targetDataOptr, std::shared_ptr<SafeONode> targetONode )
{
	std::cout << "再帰追加が実行されます" << "\n";
	

	if( citemSet()->keyCount() >= DEFAULT_KEY_COUNT )  // ノード分割が発生するケース
	{
		auto splitONode = [&]( std::shared_ptr<unsigned char> *_targetKey , std::shared_ptr<optr> *_targetDataOptr  ,std::shared_ptr<SafeONode> *_targetONode ) // targetKeyとtargetONodeは入出力引数となる
		{
			std::cout << "ノード分割が発生します" << "\n";
			ViewItemSet viewItemSet; 
			viewItemSet.importItemSet( citemSet() ); 

			viewItemSet._key.at( DEFAULT_KEY_COUNT ) = *_targetKey;
			unsigned short separatorKeyIndex = ((DEFAULT_KEY_COUNT+1)%2 == 0) ? ((DEFAULT_KEY_COUNT+1)/2)-1 : (DEFAULT_KEY_COUNT+1)/2;
			std::sort( viewItemSet._key.begin() , viewItemSet._key.end() , []( std::shared_ptr<unsigned char> a, std::shared_ptr<unsigned char> b){
				return memcmp( a.get() , b.get() , KEY_SIZE ) <= 0;
			});	 

			unsigned short keyInsertedIndex = 0; 
			for( int i=0; i<viewItemSet._key.size(); i++ )
			{
				if( memcmp( viewItemSet._key.at(i).get(), targetKey.get(), KEY_SIZE ) == 0 ){
					keyInsertedIndex = i;
					break;
				}
			}

			viewItemSet.moveInsertDataOptr( keyInsertedIndex, *_targetDataOptr ); 

			if( *_targetONode != nullptr ){ 
				viewItemSet.moveInsertChildOptr( keyInsertedIndex + 1 , (*_targetONode)->citemSet()->Optr() );
			}

			std::shared_ptr<unsigned char> separatorKey = viewItemSet._key.at( separatorKeyIndex ); 
			std::shared_ptr<optr> separatorDataOptr = viewItemSet._dataOPtr.at( separatorKeyIndex ); 
			std::shared_ptr<SafeONode> splitONode = std::make_shared<SafeONode>( nullptr );  // 新規作成

			for( int i=0; i<(DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1;i++)	{ 
				splitONode->itemSet()->key( i , viewItemSet._key.at(i+separatorKeyIndex+1) );
			}

			splitONode->itemSet()->keyCount( (DEFAULT_KEY_COUNT+1)-separatorKeyIndex-1 );
			for( int i=0; i<(DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1;i++)	{ 
				splitONode->itemSet()->dataOptr( i , viewItemSet._dataOPtr.at(i+separatorKeyIndex+1) );
			}
			splitONode->itemSet()->dataOptrCount( (DEFAULT_DATA_OPTR_COUNT+1)-separatorKeyIndex-1 );
			splitONode->itemSet()->parent( this->parent() );
			
			for( int i=0; i<separatorKeyIndex; i++)
				itemSet()->key( i , viewItemSet._key.at(i) );
			itemSet()->keyCount(separatorKeyIndex);
			for( int i=0; i<separatorKeyIndex; i++) 
				itemSet()->dataOptr( i , viewItemSet._dataOPtr.at(i) );
			itemSet()->dataOptrCount(separatorKeyIndex);

			if( citemSet()->childOptrCount() > 0 ) 
			{
				int center = (viewItemSet._childOptr.size() % 2 == 0) ? (viewItemSet._childOptr.size()/2) : ((viewItemSet._childOptr.size()/2) );
				for( int i=0; i<center; i++ ){
					itemSet()->childOptr( i , viewItemSet._childOptr.at(i) );
				}
				itemSet()->childOptrCount( center );

				for( int i=center; i<viewItemSet._childOptr.size(); i++ ){
					splitONode->itemSet()->childOptr( i - center, viewItemSet._childOptr.at(i) );
					splitONode->itemSet()->childOptrCount(i-center+1);

					if( splitONode->citemSet()->childOptr( i - center ) != nullptr ){
						splitONode->child( i - center )->itemSet()->parent( splitONode ); 
					}
				}
				splitONode->itemSet()->childOptrCount( viewItemSet._childOptr.size() - center );
			}

			*_targetKey = separatorKey;
			*_targetDataOptr = separatorDataOptr;
			*_targetONode = splitONode;

			OBtree::printONode( splitONode );
		};

		splitONode( &targetKey , &targetDataOptr ,&targetONode );

		auto parentONode = this->parent();

		unsigned char oAddrZero[5]; memset( oAddrZero, 0x00 , sizeof(oAddrZero) );

		if( memcmp( parentONode->citemSet()->Optr()->addr(), oAddrZero, 5  )  == 0 )  // 自身がルートノード場合
		{
			auto newRootNode = std::make_shared<SafeONode>( nullptr );

			/* ルートノード情報の上書き */
			std::cout << "[ @ ] (Btree::ルートノードが上書きされました)" << "\n";

			std::shared_ptr<unsigned char> metaHeadOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] ); memset( metaHeadOAddr.get() , 0x00 , 5 );
			std::shared_ptr<optr> metaHeadOptr = std::shared_ptr<optr>( new optr(metaHeadOAddr.get() , _conversionTable.safeOMemoryManager()->dataCacheTable() ) );
			omemcpy( (*metaHeadOptr) + META_ROOT_NODE_OFFSET ,  newRootNode->citemSet()->Optr()->addr() , NODE_OPTR_SIZE ); // Meta領域のルートノードの変更
	
			newRootNode->isLeaf( false );

			newRootNode->itemSet()->key( 0 ,targetKey );
			newRootNode->itemSet()->keyCount(1);

			newRootNode->itemSet()->dataOptr( 0 , targetDataOptr );
			newRootNode->itemSet()->dataOptrCount(1);

			newRootNode->itemSet()->childOptr( 0 , shared_from_this()->citemSet()->Optr() );
			newRootNode->itemSet()->childOptrCount( 1 );

			SafeOBtree::printONode( newRootNode );
			SafeOBtree::printONode( targetONode );

			newRootNode->itemSet()->childOptr( 1 ,targetONode->citemSet()->Optr() ); 
			newRootNode->itemSet()->childOptrCount(2);

			this->itemSet()->parent( newRootNode ); 
			targetONode->itemSet()->parent( newRootNode ); 
			newRootNode->itemSet()->parent( nullptr ); 

			return newRootNode;
		}
		return parentONode->recursiveAdd( targetKey, targetDataOptr ,targetONode );
	}

	else{ 
		std::cout << "単純追加が行われます" << "\n";
		itemSet()->key( citemSet()->keyCount() , targetKey );
		itemSet()->keyCount( citemSet()->keyCount() + 1 );
		itemSet()->sortKey();

		/*
		std::cout << "++++++++++++++++++++++++++++++++" << "\n";
		citemSet()->Optr()->printValueContinuously( O_NODE_ITEMSET_SIZE ); std::cout << "\n";
		std::cout << "--------------------" << "\n";
		parent()->citemSet()->Optr()->printAddr(); std::cout << "\n";
		printf("%p\n", targetONode.get() );
		targetONode->citemSet()->Optr()->printAddr(); std::cout << "\n";
		std::cout << "++++++++++++++++++++++++++++++++" << "\n";
		*/

		unsigned short keyInsertedIndex = 0; 
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

		return nullptr; 
	}

	return nullptr;
}



std::shared_ptr<SafeONode> SafeONode::remove( std::shared_ptr<unsigned char> targetKey )
{
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	int index = findIndex( targetKey );
	if( index < 0 ) return nullptr;

	if( citemSet()->childOptrCount() >= 1 ) 
	{
		std::cout << "中間ノード削除" << "\n";
		
		auto subtreeMax = child(0)->subtreeMax(); 
		itemSet()->key( index , subtreeMax->citemSet()->rawKey( subtreeMax->citemSet()->keyCount() - 1 ) ); // 削除対象にサブツリーマックス要素を追加
		itemSet()->dataOptr( index , subtreeMax->citemSet()->dataOptr( subtreeMax->citemSet()->dataOptrCount() - 1 ) );  // 削除対象にサブツリーマックス要素を追加

		auto candidateNewRootONode = subtreeMax->remove( subtreeMax->citemSet()->rawKey( subtreeMax->citemSet()->keyCount() - 1 ) );

		if  
		( 
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

	if( citemSet()->keyCount() > 1 || memcmp( parent()->citemSet()->Optr()->addr() , addrZero, sizeof(addrZero) ) == 0 )  // 単純削除のケース
	{
		itemSet()->remove( index );
		return nullptr;
	}

	if( memcmp( this->parent()->citemSet()->Optr()->addr(), addrZero , sizeof(addrZero)) == 0 )
	{
		itemSet()->moveDeleteKey(0);
		return nullptr;
	}

	return this->parent()->underflow( shared_from_this() ); // アンダーフロー発生を通知
}



std::shared_ptr<SafeONode> SafeONode::underflow( std::shared_ptr<SafeONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=citemSet()->childOptrCount() - 1; i++ ){
		index = ( memcmp( sourceONode->citemSet()->Optr()->addr(), child(i)->citemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) return nullptr;

	std::cout << "index :: " << index << "\n";
	//SafeOBtree::printONode( shared_from_this() );
	//SafeOBtree::printONode( child(index+1) );

	std::cout << "< 0 >" << "\n";
	auto targetONode = child(index);
	std::cout << "< 1 >" << "\n";
	auto leftChildONode = (index > 0) ? this->child(index-1) : nullptr; 
	std::cout << "< 2 >" << "\n";
	auto rightChildONode = this->child(index+1);

	std::cout << "< 3 >" << "\n";

	
	if( leftChildONode != nullptr && leftChildONode->citemSet()->keyCount() >= 2 ){
		std::cout << "左兄弟ノードからキーを移動" << "\n";
		sourceONode->itemSet()->key( 0 , citemSet()->rawKey(index-1) );
		sourceONode->itemSet()->keyCount(1);
		sourceONode->itemSet()->dataOptr( 0 , citemSet()->dataOptr(index-1) );
		sourceONode->itemSet()->dataOptrCount(1);

		itemSet()->key( index-1, leftChildONode->citemSet()->rawKey( leftChildONode->citemSet()->keyCount() - 1 ) );
		leftChildONode->itemSet()->moveDeleteKey( leftChildONode->citemSet()->keyCount() - 1 );
		itemSet()->dataOptr( index-1 , leftChildONode->citemSet()->dataOptr(leftChildONode->citemSet()->dataOptrCount() - 1 ) );
		leftChildONode->itemSet()->moveDeleteDataOptr( leftChildONode->citemSet()->dataOptrCount() - 1 );
		return nullptr;
	}
	if( rightChildONode != nullptr && rightChildONode->citemSet()->keyCount() >= 2 )
	{
		std::cout << "右兄弟ノードからキーを移動" << "\n";
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
	
	return merge( index );
}



std::shared_ptr<SafeONode> SafeONode::merge( std::shared_ptr<SafeONode> sourceONode )
{
	int index = -1;
	for( int i=0; i<=citemSet()->childOptrCount() - 1; i++ ){
		index = ( memcmp( sourceONode->citemSet()->Optr()->addr(), child(i)->citemSet()->Optr()->addr(), 5 ) == 0 ) ? i : -1;
		if( index != -1 ) break;
	}
	if( index == -1 ) nullptr;
	return this->merge( index );
}


std::shared_ptr<SafeONode> SafeONode::merge( unsigned short index ) // mergeが呼び出される時,兄弟ノードの各要素数は1
{
	std::cout << "\x1b[33m" << "underflow(マージ)が発生しました" << "\x1b[39m" << "\n";
	if( index == -1 ) return nullptr;

	auto targetONode = child(index);
	auto leftChildONode = (index > 0) ? this->child(index-1) : nullptr; // マイナスになる可能性があるので
	auto rightChildONode = this->child(index+1);

	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );

	if( citemSet()->keyCount() >= 2 )
	{
		if( leftChildONode != nullptr )
		{
			std::cout << "左兄弟ノードマージ" << "\n";
			itemSet()->moveDeleteChildOptr( index );

			leftChildONode->itemSet()->key( leftChildONode->citemSet()->keyCount() ,citemSet()->rawKey(index-1) );
			leftChildONode->itemSet()->keyCount( leftChildONode->citemSet()->keyCount() + 1 );
			itemSet()->moveDeleteKey(index-1);

			leftChildONode->itemSet()->dataOptr( leftChildONode->citemSet()->dataOptrCount(), citemSet()->dataOptr(index-1) );
			leftChildONode->itemSet()->dataOptrCount( leftChildONode->citemSet()->dataOptrCount() + 1 );

			return nullptr; 
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

			return nullptr;
		}
	}

	if( leftChildONode != nullptr )
	{
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

		if( memcmp( parent()->citemSet()->Optr()->addr(), addrZero, sizeof(addrZero)  ) == 0 ){
			child(0)->itemSet()->parent(nullptr);
			return child(0);
		}
		return this->parent()->recursiveMerge( shared_from_this() );
	}

	return nullptr;
}



std::shared_ptr<SafeONode> SafeONode::recursiveMerge( std::shared_ptr<SafeONode> sourceONode )
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
std::shared_ptr<SafeONode> SafeONode::recursiveMerge( unsigned short index ) // mergeが呼び出されて後に必ず呼び出される
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
		leftChildONode->itemSet()->moveDeleteChildOptr( leftChildONode->citemSet()->childOptrCount() );
		return nullptr;

	}

	if( rightChildONode != nullptr && rightChildONode->citemSet()->keyCount() >= 2 ) // 兄弟右ノードから親ノードの要素を押し出す
	{
		std::cout << "右移動再帰マージ" << "\n";
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
			return child(0); 
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








std::shared_ptr<optr> SafeONode::subtreeFind( std::shared_ptr<unsigned char> targetKey )
{
	std::shared_ptr<SafeONode> candidateChild;

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

















};
