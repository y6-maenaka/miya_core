#ifndef E73149EA_D06C_419D_B48E_FEC98444D568
#define E73149EA_D06C_419D_B48E_FEC98444D568



#include <vector>



struct SBSegment;
class StreamBuffer;





class StreamBufferContainer
{

private:
	std::vector<StreamBuffer*> _sbs;


protected:
	void pushOne( std::unique_ptr<SBSegment> target );
	std::unique_ptr<SBSegment> popOne();

public:
	StreamBufferContainer();
};











#endif // E73149EA_D06C_419D_B48E_FEC98444D568



