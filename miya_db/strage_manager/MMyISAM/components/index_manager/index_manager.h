#ifndef CD358C3C_D694_4AD5_922A_7558220922B5
#define CD358C3C_D694_4AD5_922A_7558220922B5



namespace miya_db{
class PageTableManager;



struct IndexLogicAddr
{
private:
	unsigned char _logicAddr[ 5 ]; // frame(3) + offset(2)

	void operator	+ (int offset);

}




class IndexManager{



private:

	BTree *_bTree;
	PageTableManager &_pageTableManager;

public:

	unsigned char* get();
}


}; // close miya_db namespace



#endif // CD358C3C_D694_4AD5_922A_7558220922B5

