#include "safe_btree.h"
#include "./safe_o_node.h"

#include "../components/page_table/optr_utils.h"

namespace miya_db
{





// SafeOBtree初期化時のツリーはノーマルモードのコピーで良い
SafeOBtree::SafeOBtree( std::shared_ptr<ONodeConversionTable> conversionTable ,std::shared_ptr<ONode> normalRootONode ) : OBtree( conversionTable->normalOMemoryManager() ,normalRootONode )
{
  std::cout << "this is safe obtree constructor" << "\n";
	_conversionTable = conversionTable;

	std::shared_ptr<SafeONode> dcastedSafeONode = std::shared_ptr<SafeONode>( new SafeONode(_conversionTable, normalRootONode->overlayMemoryManager(), normalRootONode->baseOptr() ) ); // 無理やり疑似的にダウンキャストしている 良くない
	_rootONode = dcastedSafeONode;
}

std::shared_ptr<ONodeConversionTable> SafeOBtree::conversionTable()
{
	return _conversionTable;
}

const std::shared_ptr<SafeONode> SafeOBtree::rootONode()
{
    return _rootONode;
}



void SafeOBtree::add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr )
{
	auto currentONode = _rootONode;
  	std::cout << "add with :: "; currentONode->hello();

	while( (currentONode->citemSet()->childOptrCount() >= 1 ) )
	{
		std::shared_ptr<optr> keyOptr;
		std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		for( int i=0; i<currentONode->citemSet()->childOptrCount(); i++ )
		{
			keyOptr = currentONode->citemSet()->key(i);
			omemcpy( rawKey.get() , keyOptr , KEY_SIZE );

			if( memcmp( targetKey.get(), rawKey.get() , KEY_SIZE ) < 0 ){
				currentONode = currentONode->child(i);
				goto direct;
			}
		}

		currentONode = currentONode->child( currentONode->citemSet()->childOptrCount()-1 );

		direct:;
	}
	std::cout << "HERE 11" << "\n";

	DataOptrEx dataOptrEx = std::make_pair( dataOptr, DATA_OPTR_LOCATED_AT_SAFE  );
	std::cout << "check point 0" << "\n";
	auto newRootNode = currentONode->recursiveAdd( targetKey , std::make_shared<DataOptrEx>(dataOptrEx) , nullptr );
	std::cout << "check point 1 " << "\n";
	if( newRootNode != nullptr )
		_rootONode = newRootNode;

	// SafeONode::_conversionTable.printEntryMap();
	std::cout << "ADD from OBtree DONE" << "\n";
	return;
}



void SafeOBtree::remove( std::shared_ptr<unsigned char> targetKey )
{
	auto currentONode = _rootONode;
	bool matchFlag = false;
	while( (currentONode->citemSet()->childOptrCount() >= 1 ) && (!matchFlag) )
	{
		std::shared_ptr<optr> keyOptr;
		std::shared_ptr<unsigned char> rawKey = std::shared_ptr<unsigned char>( new unsigned char[KEY_SIZE] );
		for( int i=0; i<currentONode->citemSet()->childOptrCount(); i++ )
		{
			keyOptr = currentONode->citemSet()->key(i);
			omemcpy( rawKey.get() , keyOptr , KEY_SIZE );

			if( memcmp( targetKey.get(), rawKey.get(), KEY_SIZE ) == 0 ) {
				currentONode = currentONode;
				matchFlag = true;
				goto direct;
			}

			if( memcmp( targetKey.get(), rawKey.get() , KEY_SIZE ) < 0 ){
				currentONode = currentONode->child(i);
				goto direct;
			}
		}

		currentONode = currentONode->child( currentONode->citemSet()->childOptrCount()-1 );

		direct:;
	}

	auto newRootNode = currentONode->remove( targetKey );
	if( newRootNode != nullptr ) // ルートノードを変更する
		_rootONode = newRootNode;
	return;
}


void SafeOBtree::hello()
{
    _rootONode->hello();
}



std::shared_ptr<DataOptrEx> SafeOBtree::find( std::shared_ptr<unsigned char> targetKey )
{
	return _rootONode->subtreeFind( targetKey );
}





std::shared_ptr<ONode> SafeOBtree::mergeSafeBtree( std::shared_ptr<SafeONode> subtreeRootONode , short int childIndex ,std::shared_ptr<ONode> parentONode )
{
	if( subtreeRootONode == nullptr ) return nullptr;  // ルートノードが存在しない場合の処理

	std::shared_ptr<optr> swapToONodeOptr;
	std::shared_ptr<ONode> swapONode;

	struct ONodeConversionTableEntryDetail retDetail;
  	retDetail = _conversionTable->refEx( subtreeRootONode->ONode::citemSet()->Optr() );


	if( !(retDetail.isExists) ) // 変更が一切無かった場合は特に何もしなくて良い
	{
		// subtreeRootONodeが持っているoptrはONodeに属するもののはず
    std::cout << "操作なし" << "\n";
    std::shared_ptr<optr> onodeOptr = subtreeRootONode->ONode::citemSet()->Optr();
    onodeOptr->cacheTable( _conversionTable->normalOMemoryManager()->dataCacheTable() );
		swapONode = std::make_shared<ONode>( _conversionTable->normalOMemoryManager() , onodeOptr );
  }

  else{
  	if( memcmp( retDetail.key->addr() , retDetail.value->_optr.addr() , 5 ) == 0 ) // SafeONOde < == > SafeONode ( 新たに領域を確保してコピーする )
  	{
      // 新たなコピー領域の割り当て
      std::cout << "SafeONode <==> SafeONode" << "\n";
  		swapToONodeOptr = _conversionTable->normalOMemoryManager()->allocate( O_NODE_ITEMSET_SIZE );
      std::cout << "New Allocated :: "; swapToONodeOptr->printAddr(); std::cout << "\n";
  		omemcpy( swapToONodeOptr.get(), &(retDetail.value->_optr) , O_NODE_ITEMSET_SIZE );

  		swapONode = std::make_shared<SafeONode>( _conversionTable , _conversionTable->normalOMemoryManager() , swapToONodeOptr );
  	}
  	else // ONode < == > SafeONode ( 既に存在する領域に上書き )
  	{
  		// ここでretDetail.entyr.firstのoptrのdataCacheが通常モードの場合でないと正常に処理が行われない
      std::cout << "ONode <==> SafeONode" << "\n";
      std::shared_ptr<optr> onodeOptr = retDetail.key; // 本来は初めからNormalのcacheTableが格納されているはずなのだが
      onodeOptr->cacheTable( _conversionTable->normalOMemoryManager()->dataCacheTable() );
  		omemcpy( onodeOptr.get() , &(retDetail.value->_optr) , O_NODE_ITEMSET_SIZE );
  		swapONode = std::make_shared<ONode>( _conversionTable->normalOMemoryManager() , onodeOptr );
  	}
  }


	if( parentONode != nullptr && childIndex >= 0)
  {
    parentONode->ONode::itemSet()->childOptr( childIndex , swapONode->ONode::citemSet()->Optr() );

    printf(" ParentONode :: %p \n childIndex :: %d\n", parentONode.get() , childIndex );
    std::cout << "ParendAddr :: "; parentONode->ONode::citemSet()->Optr()->printAddr(); std::cout << "\n";
	}
  swapONode->ONode::itemSet()->parent( parentONode ); // 親ノードのセット


  // OBtree::printONode( swapONode );

	for( int i=0; i<subtreeRootONode->citemSet()->childOptrCount(); i++ )
	{
		SafeOBtree::mergeSafeBtree( subtreeRootONode->SafeONode::child(i) , i ,swapONode );
	}

	return swapONode; // これがルートノードになる
}










int SafeOBtree::printSubTree( std::shared_ptr<SafeONode> subtreeRoot )
{
	SafeOBtree::printONode( subtreeRoot );

	for( int i=0; i<subtreeRoot->citemSet()->childOptrCount(); i++ )
	{
		SafeOBtree::printSubTree( subtreeRoot->SafeONode::child(i) );
	}

	return 0;
}



int SafeOBtree::printONode( std::shared_ptr<SafeONode> targetONode )
{
	std::cout << "\n\n============================================" << "\n";
	auto refRet = targetONode->conversionTable()->ref( targetONode->ONode::citemSet()->Optr() );
	if( refRet.second ) std::cout << "\x1b[32m";
	else std::cout << "\x1b[33m";
	targetONode->hello(); std::cout << "\n";
	std::cout << "| [ "; targetONode->citemSet()->Optr()->printAddr(); std::cout << " ]\n";
	std::cout << "| [ 親ノードアドレス ] :: ";
	targetONode->parent()->citemSet()->Optr()->printAddr(); std::cout << "\n";

	std::cout << "| [ キー個数 ] :: " << targetONode->citemSet()->keyCount() << "\n";
	for( int i=0; i<targetONode->citemSet()->keyCount(); i++ )
	{
		std::shared_ptr<unsigned char> rawKey = targetONode->citemSet()->rawKey(i);
		std::shared_ptr<optr> oKey = targetONode->citemSet()->key(i);
		std::cout << "|　 (" << i << ") "; oKey->printValueContinuously(20);  std::cout << "[";
		for( int i=0; i<20; i++ ) printf("%c", rawKey.get()[i]);
		std::cout << "]\n";
	}


	std::cout << "| [ データOポインターの個数 ] :: " << targetONode->citemSet()->dataOptrCount() << "\n";

	std::cout << "| [ 子ノード個数 ] :: " << targetONode->citemSet()->childOptrCount() << "\n";
	for( int i=0; i<targetONode->citemSet()->childOptrCount(); i++ )
	{
		auto cON = targetONode->citemSet()->childOptr(i);
    printf("%p\n", cON.get());
		std::cout << "|　 (" << i << ")" ;
		cON->printAddr(); std::cout << "\n";
	}

	std::cout << "| [ リーフノード ] :: " << targetONode->isLeaf()  << "\n";
	std::cout << "\x1b[39m";
	std::cout << "============================================\n\n" << "\n";
}



}
