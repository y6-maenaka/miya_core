#ifndef F9B61A27_5890_462E_8AD4_D47015863A43
#define F9B61A27_5890_462E_8AD4_D47015863A43


#include <memory>
#include <iostream>


// SET
// UPDATE


// jsonフォーマットでやり取りする


class MiyaDBClient
{


public:
	size_t select( std::shared_ptr<unsigned char> *ret );
	void set( std::shared_ptr<unsigned char> key , std::shared_ptr<unsigned char> data , size_t dataLength );


};




#endif // 


