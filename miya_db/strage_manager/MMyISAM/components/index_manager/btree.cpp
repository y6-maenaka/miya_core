#include "btree.h"
#include "../page_table/optr_utils.h"

#include "../page_table/overlay_memory_manager.h"
#include "./o_node.h"

namespace miya_db
{















OBtree::OBtree( std::shared_ptr<OverlayMemoryManager> oMemoryManager, std::shared_ptr<ONode> rootNode )
{
	_oMemoryManager = oMemoryManager;
	if( rootNode == nullptr )
	{
		// インデックスファイルが初期化されているか調べる
		std::shared_ptr<unsigned char> metaHeadOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] );
		memset( metaHeadOAddr.get() , 0x00 , 5 );
		std::shared_ptr<optr>	metaHeadOptr = std::shared_ptr<optr>( new optr( metaHeadOAddr.get() , oMemoryManager->dataCacheTable() ) );

		if( ocmp( metaHeadOptr.get() , (unsigned char *)FORMAT_CODE , 20 ) != 0 ) // (初期化されていない)インデックスファイルを初期化する
		{
			std::cout << "Btreeインデックスファイルの初期化されていない" << "\n";
			// ここで確保する領域がファイル先頭であることが前提 それ以外だとエラーが発生する
			std::shared_ptr<optr>	newMetaHeadOptr = oMemoryManager->allocate( 100 ); // Meta領域のサイズ分確保する

			_rootONode = std::shared_ptr<ONode>( new ONode(oMemoryManager) );  // meta領域に続いて領域を作成する
			omemcpy( (*metaHeadOptr) + META_ROOT_NODE_OFFSET , _rootONode->citemSet()->Optr()->addr() , NODE_OPTR_SIZE ); // ルートノードのセット

			omemcpy( metaHeadOptr.get() , (unsigned char *)(FORMAT_CODE) , 20 ); // フォーマットコードを設置する
			return;
		}


		std::cout << "Btreeインデックスファイルの初期は既に完了しています" << "\n";
		// Meta情報を記録する領域を作成する
		std::shared_ptr<unsigned char> rootNodeOAddr = std::shared_ptr<unsigned char>( new unsigned char[5] );
		omemcpy( rootNodeOAddr.get() , (*metaHeadOptr) + META_ROOT_NODE_OFFSET , 5 );

		std::shared_ptr<optr> rootNodeOptr = std::shared_ptr<optr>( new optr(rootNodeOAddr.get(), oMemoryManager->dataCacheTable() ));
		_rootONode = std::shared_ptr<ONode>( new ONode( oMemoryManager ,rootNodeOptr) );

		return;
	}


	_rootONode = rootNode;

}




void OBtree::rootONode( std::shared_ptr<ONode> target )
{
	_rootONode = target;

	/* 少々雑だが,メタ領域のルートノードアドレス項目も書き換える */
	unsigned char addrZero[5]; memset( addrZero , 0x00 , sizeof(addrZero) );
	std::shared_ptr<optr> metaHeaderOptr = std::make_shared<optr>(addrZero  ,_oMemoryManager->dataCacheTable() );
	if( target != nullptr ) //　空をセット
		omemcpy( (*(metaHeaderOptr.get())) + META_ROOT_NODE_OFFSET , (unsigned char*)(_rootONode->citemSet()->Optr()->addr()) , NODE_OPTR_SIZE  );
}




const std::shared_ptr<ONode> OBtree::rootONode()
{
	return _rootONode;
}




void OBtree::add( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> dataOptr )
{
	std::cout << "before_btree" << "\n";
	std::cout << "add with :: "; _rootONode->hello();
	std::cout << "after_btree" << "\n";
	auto currentONode = _rootONode;

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

	auto newRootNode = currentONode->recursiveAdd( targetKey , dataOptr , nullptr );
	if( newRootNode != nullptr )
		_rootONode = newRootNode;
	return;
}


void OBtree::remove( std::shared_ptr<unsigned char> targetKey )
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



std::shared_ptr<optr> OBtree::find( std::shared_ptr<unsigned char> targetKey )
{
	return _rootONode->subtreeFind( targetKey );
}


const std::shared_ptr<OverlayMemoryManager> OBtree::overlayMemoryManager()
{
	return _oMemoryManager;
}


int OBtree::printONode( std::shared_ptr<ONode> targetONode )
{
	std::cout << "\n\n--------------------------------------------" << "\n";
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
		std::shared_ptr<ONode> cON = targetONode->child(i);
		std::cout << "|　 (" << i << ")" ;
		cON->citemSet()->Optr()->printAddr(); std::cout << "\n";
	}

	std::cout << "| [ リーフノード ] :: " << targetONode->isLeaf()  << "\n";
	std::cout << "--------------------------------------------\n\n" << "\n";
}




int OBtree::printSubTree( std::shared_ptr<ONode> subtreeRoot )
{

	printONode( subtreeRoot );


	for( int i=0; i<subtreeRoot->citemSet()->childOptrCount(); i++ )
	{
		OBtree::printSubTree( subtreeRoot->child(i) );
	}

	return 0;
}







} // close miya_db namespace
