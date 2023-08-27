#ifndef DFCD4093_5BCF_4B44_898E_FC2E03C24740
#define DFCD4093_5BCF_4B44_898E_FC2E03C24740



#include <vector>




struct BufferElement
{
	int _flag; // ここに終了フラグが入る可能性がある
	void *_body;
	unsigned long _bodySize;
};








class SmartMiddleBuffer
{
private:
	int _bufferDescriptor;

	std::vector<BufferElement> _buffer;
	unsigned long _capacity;
	unsigned long _usage;

public:
	std::unique_ptr<BufferElement> pop();
	void push( std::unique_ptr<BufferElement> target );
};


#endif // DFCD4093_5BCF_4B44_898E_FC2E03C24740

