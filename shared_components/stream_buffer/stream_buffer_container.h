#ifndef E73149EA_D06C_419D_B48E_FEC98444D568
#define E73149EA_D06C_419D_B48E_FEC98444D568



#include <vector>
#include <functional>



struct SBSegment;
class StreamBuffer;



constexpr unsigned short MAX_SB_COUNT = 3;







class StreamBufferContainer
{

//private:
public:
	unsigned short _bufferCount;
	std::vector< std::shared_ptr<StreamBuffer> > _sbs;

	std::function<void()> _scaleOutRequest; // スケールアップフラグが立ったデータセグメントを受信したらスケールアウトする (StreamBufferの個数を増やす)
	std::function<void()> _scaleDownRequest; 


//public:


	void pushOne( std::unique_ptr<SBSegment> target );
	std::unique_ptr<SBSegment> popOne();


	StreamBufferContainer( std::function<void()> scaleOutHandler = nullptr , std::function<void()> scaleDownHandler = nullptr );
};











#endif // E73149EA_D06C_419D_B48E_FEC98444D568



