#ifndef B3A8363_EF9E_4EB6_BD4F_CE1834A3C5AD
#define B3A8363_EF9E_4EB6_BD4F_CE1834A3C5AD



template <typename T>
struct PageFrame{

}


template <typename T>
class PageCache{

private:

	int _size;

	int _index; // page_num
	 T **_addr;  // offset の数分 offset(2bytes) -> 65536 個


public:
	// int pageOut;
	// int pageIn;
	
	
};

#endif // B3A8363_EF9E_4EB6_BD4F_CE1834A3C5AD

