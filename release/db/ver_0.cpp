#include "ver_0.h"
#include "../../miya_db/strage_manager/MMyISAM/components/index_manager/b_tree.cpp"



#include "../../miya_db/strage_manager/MMyISAM/components/page_table/overlay_ptr.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/cache_manager/cache_table.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/cache_manager/mapper/mapper.h"

#include <unistd.h>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

int main(){

	/*

	MiddleBuffer buffer(100);

	unsigned char* target = (unsigned char *)"helloworld";
	unsigned int targetSize = 10;

	//buffer.bufferControl( target , targetSize );



	miya_db::BTree<unsigned char> *tree = new miya_db::BTree<unsigned char>(4);


	std::cout << "\n====== first key insert" << "\n";
	unsigned char *key_1 = (unsigned char *)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	unsigned char *dataPtr_1 = (unsigned char *)"aaaaaaaaaaaa";
	tree->_root->regist( key_1 , dataPtr_1 );

	std::cout << "\n====== second key insert" << "\n";
	unsigned char *key_2 = (unsigned char *)"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
	unsigned char *dataPtr_2 = (unsigned char *)"bbbbbbbbb";
	tree->_root->regist( key_2 , dataPtr_2 );


	std::cout << "\n====== third key insert" << "\n";
	unsigned char *key_3 = (unsigned char *)"cccccccccccccccccccccccccccccccc";
	unsigned char *dataPtr_3 = (unsigned char *)"ccccccc";
	tree->_root->regist( key_3 , dataPtr_3 );

	
	std::cout << "\n====== forth key insert" << "\n";
	unsigned char *key_4 = (unsigned char *)"dddddddddddddddddddddddddddddddd";
	unsigned char *dataPtr_4 = (unsigned char *)"ddddddd";
	tree->_root->regist( key_4 , dataPtr_4 );
	

	
	
	std::cout << "\n====== fifth key insert" << "\n";
	unsigned char *key_5 = (unsigned char *)"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
	unsigned char *dataPtr_5 = (unsigned char *)"eeeeeeee";
	tree->_root->regist( key_5 , dataPtr_5 );
	

	
	std::cout << "\n====== sixth key insert" << "\n";
	unsigned char *key_6 = (unsigned char *)"ffffffffffffffffffffffffffffffff";
	unsigned char *dataPtr_6 = (unsigned char *)"fffffff";
	tree->_root->regist( key_6 , dataPtr_6 );
	
	

	
	std::cout << "\n====== seven key insert" << "\n";
	unsigned char *key_7 = (unsigned char *)"gggggggggggggggggggggggggggggggg";
	unsigned char *dataPtr_7 = (unsigned char *)"gggggggg";
	tree->_root->regist( key_7 , dataPtr_7 );
	


	std::cout << "\n====== eigh key insert" << "\n";
	unsigned char *key_8 = (unsigned char *)"hhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
	unsigned char *dataPtr_8 = (unsigned char *)"hhhhhhhhh";
	tree->_root->regist( key_8 , dataPtr_8 );
	

	
	std::cout << "\n====== nine key insert" << "\n";
	unsigned char *key_9 = (unsigned char *)"iiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
	unsigned char *dataPtr_9 = (unsigned char *)"iiiiiiiiiiii";
	tree->_root->regist( key_9 , dataPtr_9 );



	std::cout << "\n====== ten key insert" << "\n";
	unsigned char *key_10 = (unsigned char *)"pppppppppppppppppppppppppppppppp";
	unsigned char *dataPtr_10 = (unsigned char *)"ppppppppppppppp";
	tree->_root->regist( key_10 , dataPtr_10 );
	

	
	std::cout << "\n====== ereven key insert" << "\n";
	unsigned char *key_11 = (unsigned char *)"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj";
	unsigned char *dataPtr_11 = (unsigned char *)"jjjjjjjjjjjjjjj";
	tree->_root->regist( key_11 , dataPtr_11 );


	
	std::cout << "\n====== twelve key insert" << "\n";
	unsigned char *key_12 = (unsigned char *)"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk";
	unsigned char *dataPtr_12 = (unsigned char *)"kkkkkkkkkkkkkkkk";
	tree->_root->regist( key_12 , dataPtr_12 );
	

	
	std::cout << "\n====== 13 key insert" << "\n";
	unsigned char *key_13 = (unsigned char *)"llllllllllllllllllllllllllllllll";
	unsigned char *dataPtr_13 = (unsigned char *)"llllllllllllllll";
	tree->_root->regist( key_13 , dataPtr_13 );
	

	
	std::cout << "\n====== 14 key insert" << "\n";
	unsigned char *key_14 = (unsigned char *)"rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr";
	unsigned char *dataPtr_14 = (unsigned char *)"rrrrrrrrrrrrr";
	tree->_root->regist( key_14 , dataPtr_14 );


	
	std::cout << "\n====== 15 key insert" << "\n";
	unsigned char *key_15 = (unsigned char *)"ssssssssssssssssssssssssssssssss";
	unsigned char *dataPtr_15 = (unsigned char *)"sssssssssssss";
	tree->_root->regist( key_15 , dataPtr_15 );
	

	
	std::cout << "\n====== 16 key insert" << "\n";
	unsigned char *key_16 = (unsigned char *)"tttttttttttttttttttttttttttttttt";
	unsigned char *dataPtr_16 = (unsigned char *)"tttttttttttt";
	tree->_root->regist( key_16 , dataPtr_16 );
	
	
	std::cout << "ちょっとてすと" << "\n";
	std::cout << tree->_root->isLeaf() << "\n";
	//std::cout << tree->_root->_itemSet->_itemSet._child[0]->first->isLeaf() << "\n";
	//tree->_root->_itemSet->_itemSet._child[0]->first->_isLeaf = false;
	//tree->_root->_itemSet->_itemSet._child[1]->first->_isLeaf = false;
	// splitして子ノードじゃなくなったノードのisLeafがtrueになったまま


	std::cout << "=========================== BTREE BELOW =================================" << "\n";
	printf("[ROOT] -> %p\n", tree->_root );
	std::cout << "regist to btree DONE" << "\n";
	tree->printTree(tree->_root);

	//std::cout << tree->_root->_itemSet->childCnt( true ) << "\n";

	*/



	//int indexOswapFD = open("../../miya_db/table_files/test_table/test.oswap", O_RDWR );
	int indexOswapFD = open("../miya_db/table_files/test_table/test.oswap", O_RDWR );
	miya_db::CacheTable _cacheTable( indexOswapFD );
	miya_db::Mapper *_mapper = _cacheTable.mapper();
	// std::cout << _mapper->fd() << "\n";
	

	unsigned char initAddr = 0x00;
	miya_db::optr _optr(initAddr);

	unsigned char target = 0xAA;
	_optr.value( target );

}
