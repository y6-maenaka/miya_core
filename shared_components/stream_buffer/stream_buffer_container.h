#ifndef E73149EA_D06C_419D_B48E_FEC98444D568
#define E73149EA_D06C_419D_B48E_FEC98444D568



#include <vector>


class SegmentBuffer;






class StreamBufferContainer
{

private:
	std::vector<SegmentBuffer> _sbs;


protected:
	void upScale();
	void downScale();

};











#endif // E73149EA_D06C_419D_B48E_FEC98444D568



