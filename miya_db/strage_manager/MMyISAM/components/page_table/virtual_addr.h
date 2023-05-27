#ifndef EDE5692A_1133_4138_9D45_89CC0A01ED81
#define EDE5692A_1133_4138_9D45_89CC0A01ED81



namespace miya_db{

struct VirtualAddr
{

private:
	unsigned char _virtualAddr[5];
	void *physicalAddr;

public:
	unsigned char *frame();
	unsigned char *offset();

};





}; // close miya_db namespace


#endif // EDE5692A_1133_4138_9D45_89CC0A01ED81

