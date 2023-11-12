#include "safe_btree.h"
#include "./safe_o_node.h"

#include "../../components/page_table/optr_utils.h"

namespace miya_db
{






SafeOBtree::SafeOBtree( std::shared_ptr<ONode> normalRootONode ) : OBtree( SafeONode::_conversionTable.normalOMemoryManager() ,normalRootONode )
{
    // static_pointer_castを使用すると基底クラスポインタとなり，SafeNodeでオーバーロードしているメソッドが使えない
    SafeONode *castedNormalONode = static_cast<SafeONode*>( normalRootONode.get() );
    _rootONode = std::make_shared<SafeONode>( *castedNormalONode );
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


	auto newRootNode = currentONode->recursiveAdd( targetKey , dataOptr , nullptr );
	if( newRootNode != nullptr )
		_rootONode = newRootNode;


	SafeONode::_conversionTable.printEntryMap();
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
	auto refRet = SafeONode::_conversionTable.ref( targetONode->ONode::citemSet()->Optr() );
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
		auto cON = targetONode->child(i);
		std::cout << "|　 (" << i << ")" ;
		cON->citemSet()->Optr()->printAddr(); std::cout << "\n";
	}

	std::cout << "| [ リーフノード ] :: " << targetONode->isLeaf()  << "\n";
	std::cout << "\x1b[39m";
	std::cout << "============================================\n\n" << "\n";
}



}
