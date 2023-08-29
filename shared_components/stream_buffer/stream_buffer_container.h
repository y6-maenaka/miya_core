#ifndef E73149EA_D06C_419D_B48E_FEC98444D568
#define E73149EA_D06C_419D_B48E_FEC98444D568



#include <vector>
#include <functional>



struct SBSegment;
class StreamBuffer;



constexpr unsigned short MAX_SB_COUNT = 3;







class StreamBufferContainer
{

private:
	unsigned short _bufferCount;
	std::vector<StreamBuffer*> _sbs;


	std::function<void()> _scaleOutRequest;
	std::function<void()> _scaleDownRequest;


protected:
	void pushOne( std::unique_ptr<SBSegment> target );
	std::unique_ptr<SBSegment> popOne();

public:
	StreamBufferContainer( std::function<void()> scaleOutHandler , std::function<void()> scaleDownHandler );
};











#endif // E73149EA_D06C_419D_B48E_FEC98444D568



