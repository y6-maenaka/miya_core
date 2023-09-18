#ifndef CD3D3C5E_6902_4CE2_A46C_9142BF794829
#define CD3D3C5E_6902_4CE2_A46C_9142BF794829


#include <memory>
#include <string>
#include <variant>
#include <iostream>


namespace tx
{
	struct P2PKH;
}




class ControlInterface
{
public:

	
	std::shared_ptr<tx::P2PKH> createTxFromJsonFile( const char *filePath );


};




#endif // CD3D3C5E_6902_4CE2_A46C_9142BF794829



