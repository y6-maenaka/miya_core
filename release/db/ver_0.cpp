#include "ver_0.h"
#include "../../miya_db/strage_manager/MMyISAM/components/index_manager/btree.h"



#include "../../miya_db/strage_manager/MMyISAM/components/page_table/overlay_ptr.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/cache_manager/cache_table.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/cache_manager/mapper/mapper.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/overlay_memory_manager.h"
#include "../../miya_db/strage_manager/MMyISAM/components/page_table/optr_utils.h"



#include <unistd.h>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

int main(){

	

	MiddleBuffer buffer(100);

	unsigned char* target = (unsigned char *)"helloworld";
	unsigned int targetSize = 10;

	miya_db::OBtree btree;
	std::cout << btree.rootNode() << "\n";

	//buffer.bufferControl( target , targetSize );

	/*
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
	/*
	int indexOswapFD = open("../miya_db/table_files/test_table/test.oswap", O_RDWR , (mode_t)0600 );
	miya_db::CacheTable _cacheTable( indexOswapFD );
	miya_db::Mapper *_mapper = _cacheTable.mapper();
	// std::cout << _mapper->fd() << "\n";
	

	std::cout << "====================================" << "\n";
	unsigned char initAddr[5] = {0x00 , 0x00 , 0x02, 0x00, 0x05};
	miya_db::optr _optr( initAddr );
	_optr.cacheTable( &_cacheTable );
	std::cout << "Frame -> " << _optr.frame() << "\n";
	std::cout << _optr.offset() << "\n";
	_optr.value('B'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";




	std::cout << "====================================" << "\n";
	unsigned char initAddr2[5] = {0x00 , 0x00 , 0x02, 0x00 , 0x00};
	miya_db::optr _optr2( initAddr2 );
	_optr2.cacheTable( &_cacheTable );
	std::cout << "Frame -> " << _optr2.frame() << "\n";
	std::cout << _optr2.offset() << "\n";
	_optr2.value('C'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";



	std::cout << "====================================" << "\n";
	unsigned char initAddr3[5] = {0x00 , 0x00 , 0x00, 0x00 , 0x02};
	miya_db::optr _optr3( initAddr3 );
	_optr3.cacheTable( &_cacheTable );
	std::cout << "Frame -> "<< _optr3.frame() << "\n";
	std::cout << _optr3.offset() << "\n";
	_optr3.value('X'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";


	std::cout << "====================================" << "\n";
	unsigned char initAddr4[5] = {0x00 , 0x00 , 0x01, 0x00 , 0x02};
	miya_db::optr _optr4( initAddr4 );
	_optr4.cacheTable( &_cacheTable );
	std::cout << "Frame -> "<< _optr4.frame() << "\n";
	std::cout << _optr4.offset() << "\n";
	_optr4.value('X'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";



	std::cout << "====================================" << "\n";
	unsigned char initAddr5[5] = {0x00 , 0x00 , 0x03, 0x00 , 0x02};
	miya_db::optr _optr5( initAddr5 );
	_optr5.cacheTable( &_cacheTable );
	std::cout << "Frame -> "<< _optr5.frame() << "\n";
	std::cout << _optr5.offset() << "\n";
	_optr5.value('X'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";


	std::cout << "====================================" << "\n";
	unsigned char initAddr6[5] = {0x00 , 0x00 , 0x03, 0x00 , 0x03};
	miya_db::optr _optr6( initAddr6 );
	_optr6.cacheTable( &_cacheTable );
	std::cout << "Frame -> "<< _optr6.frame() << "\n";
	std::cout << _optr6.offset() << "\n";
	_optr6.value('X'); // ファイル上の1バイト格納空間のオーバレイポインタ
	
	_cacheTable.cacheingList();
	_cacheTable.invalidList();
	std::cout << "====================================" << "\n\n";


	char *testStrings = "Hello World";
	omemcpy( &_optr3 , testStrings , 11 );

	printf("%p\n", &(_optr));
	*/


	
	int indexOswapFD = open("../miya_db/table_files/test_table/test.oswap", O_RDWR , (mode_t)0600 );
	miya_db::CacheTable _cacheTable( indexOswapFD );
	miya_db::Mapper *_mapper = _cacheTable.mapper();
	// std::cout << _mapper->fd() << "\n";

	unsigned char initAddr[5] = {0x00 , 0x00 , 0x00, 0x00, 0x00};
	miya_db::optr _optr( initAddr );
	_optr.cacheTable( &_cacheTable );

	unsigned char initAddr2[5] = {0x00 , 0x00 , 0x00, 0x00, 0x10};
	miya_db::optr _optr2( initAddr2 );
	_optr2.cacheTable( &_cacheTable );



	_cacheTable.cacheingList();
	_cacheTable.invalidList();
			
	char *greet = "Hello World!";
	char *hello = "HzZZZ"; // こっちの方が大きい
												 
	omemcpy( &_optr , greet , 5);
	omemcpy( &_optr2 , hello , 5 );
	int flag = ocmp( &_optr , &_optr2 , 5 );
	std::cout << "flag -> " << flag << "\n";

}
